#include <math.h>

#include <stdint.h>

#include "usbd_cdc_if.h"

#include "Bridge.h"

#include "Data_Acquisition_Module.h"

extern uint8_t Rx_Buf[RX_BUF_SZ];
extern uint8_t Tx_Buf[TX_BUF_SZ];

extern int FLAG_New_Data_Received;

float ADC_Resolution = 1 << ADC_Bits;
float ADC_U_Ref = 5.0; //V



double I_TXunit = 0.0005;  //  A/unit
double U_TXunit = 0.0025; //  V/unit
double t_TXunit = 0.01;    // oC/unit
double I_TX_1st_byte = 0.1; //  A/unit
double U_TX_1st_byte = 0.5; //  V/unit
double t_TX_1st_byte = 1.0; // oC/unit

double dI_dUADC = 0.4; // 0.4 A in sensor equal to 1 V_adc
double dU_dUADC = 0.1; // 10.0 V in sensor equal to 1 V_adc
double dt_dUADC = 0.1; // See documentation


extern int FLAG_New_Data_Received;



void Convert_ADC_to_Tx_format( s_ADC *stm32ADC,  s_Tx_Parameters *Tx_Prms) {
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


	I_ADCunit = ADC_U_Ref / (dI_dUADC * ADC_Resolution);
	U_ADCunit = ADC_U_Ref / (dU_dUADC * ADC_Resolution);
	t_ADCunit = ADC_U_Ref / (dt_dUADC * ADC_Resolution);

	K_I_adc_to_rx = I_ADCunit / I_TXunit;
	K_U_adc_to_rx = U_ADCunit / U_TXunit;
	K_t_adc_to_rx = t_ADCunit / t_TXunit;

	I_in__Utx = K_I_adc_to_rx * (double)stm32ADC->I_in;
	U_in__Utx = K_U_adc_to_rx * (double)stm32ADC->U_in;
	I_out_Utx = K_I_adc_to_rx * (double)stm32ADC->I_out;
	U_out_Utx = K_U_adc_to_rx * (double)stm32ADC->U_out;
	t_1___Utx = K_t_adc_to_rx * (double)stm32ADC->t_1;
	t_2___Utx = K_t_adc_to_rx * (double)stm32ADC->t_2;
	t_3___Utx = K_t_adc_to_rx * (double)stm32ADC->t_3;

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

void Create_Tx_Buffer( s_Tx_Parameters* Tx_Prms, uint8_t TX_Buf[TX_BUF_SZ]) {
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


void On_PC_Create_Rx_Parcel( s_Rx_Parameters* Rx_Parameters, uint8_t RX_Buf[RX_BUF_SZ]) {

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

	RX_Buf[0] = (uint8_t)Head                    ;
	RX_Buf[1] = (uint8_t)Rx_Parameters->t_Seed   ;
	RX_Buf[2] = (uint8_t)Rx_Parameters->t_Period ;
	RX_Buf[3] = (uint8_t)Rx_Parameters->t_Strobe ;
	RX_Buf[4] = (uint8_t)Rx_Parameters->t_Delay  ;
	RX_Buf[5] = (uint8_t)Rx_Parameters->t_Active ;
	RX_Buf[6] = (uint8_t)Rx_Parameters->t_ADC    ;
	RX_Buf[7] = (uint8_t)Rx_Parameters->N_Periods;
	RX_Buf[8] = (uint8_t)Tail                    ;
}
int Process_Rx_Parcel( s_Rx_Parameters* Rx_Parameters, uint8_t RX_Buf[RX_BUF_SZ]) {
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

void Initialize_System( s_System* Sys) {
	Sys->InPeriod_cntr = 0;
	Sys->Periods_cntr  = 0;
	Sys->Seeds_cntr    = 0;
	Sys->ADC_cntr      = 0;
	Sys->Strobe = 0;
	Sys->DCDC_On = 0;
}

void Initialize_System2(s_Rx_Parameters* Rx_Parameters) {
	Rx_Parameters->t_Seed = 0x30;
	Rx_Parameters->t_Period = 0xF8;
	Rx_Parameters->t_Strobe = 0x01;
	Rx_Parameters->t_Delay = 0x0A;
	Rx_Parameters->t_Active = 0x04;
	Rx_Parameters->t_ADC = 0xFA;
	Rx_Parameters->N_Periods = 251;
}

void Timer_100us( s_Rx_Parameters* Rx_Parameters,
		          uint8_t RX_Buf[RX_BUF_SZ],
	              s_System *Sys,
	              s_ADC* stm32ADC,
	              s_Tx_Parameters* Tx_Parameters,
				  uint8_t TX_Buf[TX_BUF_SZ]
	) {
	if (FLAG_New_Data_Received == 1) {
		Initialize_System(Sys);
		Process_Rx_Parcel(Rx_Parameters, RX_Buf);
	}
	if ( (Sys->Periods_cntr >= 0) && (Sys->Periods_cntr < Rx_Parameters->N_Periods) ){
		if (Sys->ADC_cntr >= Rx_Parameters->t_ADC) {

			static int kk = 0;
//			int A;
//			uint8_t A1[2];

			Sys->ADC_cntr = 0;
//			ADC_Measurement_Imitation(ADC);
//			A = Run_ADC();

			All_Parameters_Measurement(stm32ADC);

		    Convert_ADC_to_Tx_format(stm32ADC, Tx_Parameters);
		    Create_Tx_Buffer(Tx_Parameters, (uint8_t*)TX_Buf);

//		    A1[0] = 0xFF & A;
//		    A1[1] = 0xFF & (A >> 8);

		    if(kk >= 30){
//		        CDC_Transmit_FS(A1, 2);
		        kk = 0;
			TxBuffer_Send(Tx_Buf);
		    }
		    kk = kk + 1;
		}
		Sys->ADC_cntr = Sys->ADC_cntr + 1;
		if (Sys->Seeds_cntr >= Rx_Parameters->t_Seed) {
			Sys->Seeds_cntr = 0;
		
			Sys->ADC_cntr = Sys->ADC_cntr + 1;
			if (Sys->InPeriod_cntr >= Rx_Parameters->t_Period) {
				Sys->InPeriod_cntr = 0;
				Sys->Strobe = 1;
				Sys->DCDC_On = 0;
				if(Rx_Parameters->N_Periods != 251){
				    Sys->Periods_cntr = Sys->Periods_cntr + 1;
				}
				Set_Sync_Pin();
				Clr_DCDC_En_Pin();
			}
			if (Sys->InPeriod_cntr == Rx_Parameters->t_Strobe) {
				Sys->Strobe = 0;
				Clr_Sync_Pin();

			}
			if (Sys->InPeriod_cntr == Rx_Parameters->t_Delay) {
				Sys->DCDC_On = 1;
				Set_DCDC_En_Pin();
			}
			if (Sys->InPeriod_cntr == Rx_Parameters->t_Delay + Rx_Parameters->t_Active) {
				Sys->DCDC_On = 0;
				Clr_DCDC_En_Pin();
			}
			Sys->InPeriod_cntr = Sys->InPeriod_cntr + 1;

		}
		Sys->Seeds_cntr = Sys->Seeds_cntr + 1;

	}
}
