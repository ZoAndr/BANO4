#ifndef DATA_ACQUISITION_MODULE_H
#define DATA_ACQUISITION_MODULE_H

#define ADC_Bits  12

#define RX_BUF_SZ 16
#define TX_BUF_SZ 64

#include <machine/_default_types.h>

typedef struct {
	int A1[8];
	int A2[8];
	int A3[8];
} s_HW_ADC;

typedef struct  {
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
} s_Tx_Parameters;

typedef struct  {
	int I_in;
	int U_in;
	int I_out;
	int U_out;
	int t_1;
	int t_2;
	int t_3;
} s_ADC;

typedef struct  {
	int t_Seed;
	int t_Period;
	int t_Strobe;
	int t_Delay;
	int t_Active;
	int t_ADC;
	int N_Periods;
} s_Rx_Parameters;

typedef struct  {
	int Seeds_cntr;
	int InPeriod_cntr;
	int Periods_cntr;
	int ADC_cntr;
	int Strobe;
	int DCDC_On;
} s_System;

void Convert_ADC_to_Tx_format( s_ADC *stm32ADC, s_Tx_Parameters *Tx_Prms);
void Create_Tx_Buffer( s_Tx_Parameters* Tx_Prms, uint8_t TX_Buf[TX_BUF_SZ]);
void On_PC_Create_Rx_Parcel( s_Rx_Parameters* Rx_Parameters, uint8_t RX_Buf[RX_BUF_SZ]);
int Process_Rx_Parcel( s_Rx_Parameters* Rx_Parameters, uint8_t RX_Buf[RX_BUF_SZ]);
void Initialize_System( s_System* Sys);
void Initialize_System2(s_Rx_Parameters* Rx_Parameters);
void Timer_100us( s_Rx_Parameters* Rx_Parameters,
		         uint8_t RX_Buf[RX_BUF_SZ],
	             s_System *System,
	             s_ADC* stm32ADC,
	             s_Tx_Parameters* Tx_Parameters,
				 uint8_t TX_Buf[TX_BUF_SZ]
	);

#endif
