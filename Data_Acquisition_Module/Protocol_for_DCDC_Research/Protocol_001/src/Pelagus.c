#include <stdio.h>
#include <math.h>

#define ADC_Bits  12

#define RX_BUF_SZ 16
#define TX_BUF_SZ 64

char Rx_Buf[RX_BUF_SZ];
char Tx_Buf[TX_BUF_SZ];

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

void Measurement(struct s_ADC* ADC) {

	float ADC_Gain;

	float I_in__Uadc;
	float U_in__Uadc;
	float I_out_Uadc;
	float U_out_Uadc;
	float t_1___Uadc;
	float t_2___Uadc;
	float t_3___Uadc;

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

void convert_ADC_to_Tx_format(struct s_ADC *ADC, struct s_Tx_Parameters *Tx_Prms) {
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

void process_Rx_Parcel(struct s_Rx_Parameters* Rx_Parameters, char RX_Buf[RX_BUF_SZ]) {
	int a;
}

void main() {
	struct s_Tx_Parameters Tx_Parameters;
	struct s_Rx_Parameters Rx_Parameters;
	struct s_ADC ADC;

	Measurement(&ADC);
	convert_ADC_to_Tx_format(&ADC, &Tx_Parameters);
}