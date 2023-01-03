#ifndef BRIDGE_H
#define BRIDGE_H


#define SYNC_PORT GPIOD
#define SYNC_PIN  GPIO_PIN_13
#define DCDC_EN_PORT GPIOD
#define DCDC_EN_PIN  GPIO_PIN_14

#define U_IN_ADC 1
#define U_IN_PIN ADC_CHANNEL_0
#define I_IN_ADC 1
#define I_IN_PIN ADC_CHANNEL_1
#define U_OUT_ADC 1
#define U_OUT_PIN ADC_CHANNEL_2
#define I_OUT_ADC 2
#define I_OUT_PIN ADC_CHANNEL_0
#define T_1_ADC 2
#define T_1_PIN ADC_CHANNEL_1
#define T_2_ADC 2
#define T_2_PIN ADC_CHANNEL_2
#define T_3_ADC 3
#define T_3_PIN ADC_CHANNEL_0

#include "Data_Acquisition_Module.h"

void Set_Sync_Pin();
void Clr_Sync_Pin();
void Set_DCDC_En_Pin();
void Clr_DCDC_En_Pin();

int Run_ADC();
int  ADC_Measurement(int N_ADC, int ADC_CHANNEL);
void All_Parameters_Measurement(s_ADC* stm32ADC);

void TxBuffer_Send(uint8_t* Tx_Buf);

#endif
