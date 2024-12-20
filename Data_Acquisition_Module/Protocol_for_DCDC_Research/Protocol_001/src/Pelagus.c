#include <stdio.h>
#include <math.h>

#define ADC_Bits  12

#define RX_BUF_SZ 16
#define TX_BUF_SZ 64

char Rx_Buf[RX_BUF_SZ];
char Tx_Buf[TX_BUF_SZ];

int FLAG_New_Data_Received = 0;

double dI_dUADC = 0.4; // 0.4 A in sensor equal to 1 V_adc
double dU_dUADC = 0.1; // 10.0 V in sensor equal to 1 V_adc
double dt_dUADC = 0.1; // See documentation

double I_in  = 3.0; // A
double U_in  = 10.0; // V
double I_out = 1.0; // A
double U_out = 30.0; // V
double t_1  = 25.62; // oC
double t_2  = 32.16; // oC
double t_3  = 65.19; // oC

double I_TXunit = 0.0005;  //  A/unit
double U_TXunit = 0.0025; //  V/unit
double t_TXunit = 0.01;    // oC/unit
double I_TX_1st_byte = 0.1; //  A/unit
double U_TX_1st_byte = 0.5; //  V/unit
double t_TX_1st_byte = 1.0; // oC/unit

float ADC_Resolution = 1 << ADC_Bits;
float ADC_U_Ref = 5.0; //V


struct s_Tx_Parameters {
	int I_in_1;
	int I_in_2;
	int U_in_1;
	int U_in_2;
	int I_out_1;
	int I_out_2;
	int U_out_1;
	int U_out_2;
	int T1_1;
	int T1_2;
	int T2_1;
	int T2_2;
	int T3_1;
	int T3_2;
};

struct s_ADC {
	int I_in;
	int U_in;
	int I_out;
	int U_out;
	int t_1;
	int t_2;
	int t_3;
};

struct s_Rx_Parameters {
	int t_Seed;
	int t_Period;
	int t_Strobe;
	int t_Delay;
	int t_Active;
	int t_ADC;
	int N_Periods;
};

struct s_System {
	int Seeds_cntr;
	int InPeriod_cntr;
	int Periods_cntr;
	int ADC_cntr;
	int Strobe;
	int DCDC_On;
};

void ADC_Measurement_Imitation(struct s_ADC* ADC) {

	double ADC_Gain;

	double I_in__Uadc;
	double U_in__Uadc;
	double I_out_Uadc;
	double U_out_Uadc;
	double t_1___Uadc;
	double t_2___Uadc;
	double t_3___Uadc;

	ADC_Gain = ADC_Resolution / ADC_U_Ref;

	I_in__Uadc = I_in  * dI_dUADC;
	U_in__Uadc = U_in  * dU_dUADC;
	I_out_Uadc = I_out * dI_dUADC;
	U_out_Uadc = U_out * dU_dUADC;
	t_1___Uadc = t_1   * dt_dUADC;
	t_2___Uadc = t_2   * dt_dUADC;
	t_3___Uadc = t_3   * dt_dUADC;

    ADC->I_in  = (int)( I_in__Uadc * ADC_Gain );
    ADC->U_in  = (int)( U_in__Uadc * ADC_Gain );
    ADC->I_out = (int)( I_out_Uadc * ADC_Gain );
    ADC->U_out = (int)( U_out_Uadc * ADC_Gain );
    ADC->t_1   = (int)( t_1___Uadc * ADC_Gain );
    ADC->t_2   = (int)( t_2___Uadc * ADC_Gain );
    ADC->t_3   = (int)( t_3___Uadc * ADC_Gain );
}

void Convert_ADC_to_Tx_format(struct s_ADC *ADC, struct s_Tx_Parameters *Tx_Prms) {
	// Calculation of ADC's units:
	double I_ADCunit;
	double U_ADCunit;
	double t_ADCunit;

	double I_in__Utx;
	double U_in__Utx;
	double I_out_Utx;
	double U_out_Utx;
	double t_1___Utx;
	double t_2___Utx;
	double t_3___Utx;

	double K_I_adc_to_rx;
	double K_U_adc_to_rx;
	double K_t_adc_to_rx;

	double K_I_1st_byte; // number of units in second byte for 1 unit in first byte
	double K_U_1st_byte; // number of units in second byte for 1 unit in first byte
	double K_t_1st_byte; // number of units in second byte for 1 unit in first byte

	double K;

	I_ADCunit = ADC_U_Ref / (dI_dUADC * ADC_Resolution);
	U_ADCunit = ADC_U_Ref / (dU_dUADC * ADC_Resolution);
	t_ADCunit = ADC_U_Ref / (dt_dUADC * ADC_Resolution);

	K_I_adc_to_rx = I_ADCunit / I_TXunit;
	K_U_adc_to_rx = U_ADCunit / U_TXunit;
	K_t_adc_to_rx = t_ADCunit / t_TXunit;

	I_in__Utx = K_I_adc_to_rx * (double)ADC->I_in;
	U_in__Utx = K_U_adc_to_rx * (double)ADC->U_in;
	I_out_Utx = K_I_adc_to_rx * (double)ADC->I_out;
	U_out_Utx = K_U_adc_to_rx * (double)ADC->U_out;
	t_1___Utx = K_t_adc_to_rx * (double)ADC->t_1;
	t_2___Utx = K_t_adc_to_rx * (double)ADC->t_2;
	t_3___Utx = K_t_adc_to_rx * (double)ADC->t_3;

    // Split data for 1st and 2nd bytes to transmit

	K_I_1st_byte = (I_TX_1st_byte / I_TXunit);
	K_U_1st_byte = (U_TX_1st_byte / U_TXunit);
	K_t_1st_byte = (t_TX_1st_byte / t_TXunit);

	Tx_Prms->I_in_1 = (int)(I_in__Utx / K_I_1st_byte);
	Tx_Prms->I_in_2 = (int)(I_in__Utx - (double)Tx_Prms->I_in_1 * K_I_1st_byte);
	Tx_Prms->U_in_1 = (int)(U_in__Utx / K_U_1st_byte);
	Tx_Prms->U_in_2 = (int)(U_in__Utx - (double)Tx_Prms->U_in_1 * K_U_1st_byte);
	Tx_Prms->I_out_1 = (int)(I_out_Utx / K_I_1st_byte);
	Tx_Prms->I_out_2 = (int)(I_out_Utx - (double)Tx_Prms->I_out_1 * K_I_1st_byte);
	Tx_Prms->U_out_1 = (int)(U_out_Utx / K_U_1st_byte);
	Tx_Prms->U_out_2 = (int)(U_out_Utx - (double)Tx_Prms->U_out_1 * K_U_1st_byte);

	Tx_Prms->T1_1 = (int)(t_1___Utx / K_t_1st_byte);
	Tx_Prms->T1_2 = (int)(t_1___Utx - (double)Tx_Prms->T1_1 * K_t_1st_byte);
	Tx_Prms->T2_1 = (int)(t_2___Utx / K_t_1st_byte);
	Tx_Prms->T2_2 = (int)(t_2___Utx - (double)Tx_Prms->T2_1 * K_t_1st_byte);
	Tx_Prms->T3_1 = (int)(t_3___Utx / K_t_1st_byte);
	Tx_Prms->T3_2 = (int)(t_3___Utx - (double)Tx_Prms->T3_1 * K_t_1st_byte);

}

void Create_Tx_Buffer(struct s_Tx_Parameters* Tx_Prms, char TX_Buf[TX_BUF_SZ]) {
	int Head, Tail;
	Head = 0xFF;
	Tail = 0xFE;
	TX_Buf[ 0] = Head;
	TX_Buf[ 1] = Tx_Prms->I_in_1;
	TX_Buf[ 2] = Tx_Prms->I_in_2;
	TX_Buf[ 3] = Tx_Prms->U_in_1;
	TX_Buf[ 4] = Tx_Prms->U_in_2;
	TX_Buf[ 5] = Tx_Prms->I_out_1;
	TX_Buf[ 6] = Tx_Prms->I_out_2;
	TX_Buf[ 7] = Tx_Prms->U_out_1;
	TX_Buf[ 8] = Tx_Prms->U_out_2;
	TX_Buf[ 9] = Tx_Prms->T1_1;
	TX_Buf[10] = Tx_Prms->T1_2;
	TX_Buf[11] = Tx_Prms->T2_1;
	TX_Buf[12] = Tx_Prms->T2_2;
	TX_Buf[13] = Tx_Prms->T3_1;
	TX_Buf[14] = Tx_Prms->T3_2;
	TX_Buf[15] = Tail;
}


int On_PC_Create_Rx_Parcel(struct s_Rx_Parameters* Rx_Parameters, char RX_Buf[RX_BUF_SZ]) {

	int Head, Tail;
	Head = 0xFF;
	Tail = 0xFE;

	Rx_Parameters->t_Seed    =  5;
	Rx_Parameters->t_Period  = 20;
	Rx_Parameters->t_Strobe  =  3;
	Rx_Parameters->t_Delay   = 12;
	Rx_Parameters->t_Active  =  4;
	Rx_Parameters->t_ADC     = 10;
	Rx_Parameters->N_Periods =  3;

	RX_Buf[0] = (char)Head                    ;
	RX_Buf[1] = (char)Rx_Parameters->t_Seed   ;
	RX_Buf[2] = (char)Rx_Parameters->t_Period ;
	RX_Buf[3] = (char)Rx_Parameters->t_Strobe ;
	RX_Buf[4] = (char)Rx_Parameters->t_Delay  ;
	RX_Buf[5] = (char)Rx_Parameters->t_Active ;
	RX_Buf[6] = (char)Rx_Parameters->t_ADC    ;
	RX_Buf[7] = (char)Rx_Parameters->N_Periods;
	RX_Buf[8] = (char)Tail                    ;
}
int Process_Rx_Parcel(struct s_Rx_Parameters* Rx_Parameters, char RX_Buf[RX_BUF_SZ]) {
	int error;
	int Head, Tail;
	Head                     = (int)RX_Buf[0];
	Rx_Parameters->t_Seed    = (int)RX_Buf[1];
	Rx_Parameters->t_Period  = (int)RX_Buf[2];
	Rx_Parameters->t_Strobe  = (int)RX_Buf[3];
	Rx_Parameters->t_Delay   = (int)RX_Buf[4];
	Rx_Parameters->t_Active  = (int)RX_Buf[5];
	Rx_Parameters->t_ADC     = (int)RX_Buf[6];
	Rx_Parameters->N_Periods = (int)RX_Buf[7];
	Tail                     = (int)RX_Buf[8];
	if ((Head == 0xFF) && (Tail == 0xFE)) {	error = 0; FLAG_New_Data_Received = 0; }
	else { error = 1; }
	return error;
}

void Initialize_System(struct s_System* System) {
	System->InPeriod_cntr = 0;
	System->Periods_cntr  = 0;
	System->Seeds_cntr    = 0;
	System->ADC_cntr      = 0;
	System->Strobe = 0;
	System->DCDC_On = 0;
}

void Read_ADCs(struct s_ADC* ADC) {
}

void TxBuffer_Send(char TX_Buf[TX_BUF_SZ]) {
}

void Timer_100us(struct s_Rx_Parameters* Rx_Parameters, 
	             char RX_Buf[RX_BUF_SZ], 
	             struct s_System *System,
	             struct s_ADC* ADC,
	             struct s_Tx_Parameters* Tx_Parameters,
	             char TX_Buf[TX_BUF_SZ]
	) {
	if (FLAG_New_Data_Received == 1) {
		Initialize_System(&System);
		Process_Rx_Parcel(Rx_Parameters, RX_Buf);
	}
	if ( (System->Periods_cntr >= 0) && (System->Periods_cntr < Rx_Parameters->N_Periods) ){
		if (System->ADC_cntr >= Rx_Parameters->t_ADC) {
			System->ADC_cntr = 0;
			ADC_Measurement_Imitation(ADC);
		    Convert_ADC_to_Tx_format(ADC, Tx_Parameters);
		    Create_Tx_Buffer(Tx_Parameters, (char*)TX_Buf);
			TxBuffer_Send((char*)TX_Buf);
		}
		System->ADC_cntr = System->ADC_cntr + 1;
		if (System->Seeds_cntr >= Rx_Parameters->t_Seed) {
			System->Seeds_cntr = 0;
		
			System->ADC_cntr = System->ADC_cntr + 1;
			if (System->InPeriod_cntr >= Rx_Parameters->t_Period) {
				System->InPeriod_cntr = 0;
				System->Strobe = 1;
				System->DCDC_On = 0;
				System->Periods_cntr = System->Periods_cntr + 1;
			}
			if (System->InPeriod_cntr == Rx_Parameters->t_Strobe) {
				System->Strobe = 0;
			}
			if (System->InPeriod_cntr == Rx_Parameters->t_Delay) {
				System->DCDC_On = 1;
			}
			if (System->InPeriod_cntr == Rx_Parameters->t_Delay + Rx_Parameters->t_Active) {
				System->DCDC_On = 0;
			}
			System->InPeriod_cntr = System->InPeriod_cntr + 1;

		}
		System->Seeds_cntr = System->Seeds_cntr + 1;

	}
}



void main() {
	struct s_Tx_Parameters Tx_Parameters;
	struct s_Rx_Parameters Rx_Parameters;
	struct s_ADC ADC;
	struct s_System System;

	ADC_Measurement_Imitation(&ADC);
	Convert_ADC_to_Tx_format(&ADC, &Tx_Parameters);
	Create_Tx_Buffer(&Tx_Parameters, (char*)&Tx_Buf);

	Initialize_System(&System);

	On_PC_Create_Rx_Parcel(&Rx_Parameters, (char*)&Rx_Buf);
	Process_Rx_Parcel(&Rx_Parameters, (char*)&Rx_Buf);

	int t_k = 0;

	while (1) {

		Timer_100us(&Rx_Parameters, (char*)&Rx_Buf,
			&System, &ADC, &Tx_Parameters, (char*)&Tx_Buf);

		if (t_k++ > 10000) { return; }
	}

}