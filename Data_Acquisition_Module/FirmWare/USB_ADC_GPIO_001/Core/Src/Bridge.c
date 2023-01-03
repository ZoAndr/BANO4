
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"

#include "Bridge.h"
#include "Data_Acquisition_Module.h"

#define TX_DATA_SZ 16

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

void Set_Sync_Pin(){
	HAL_GPIO_WritePin (SYNC_PORT, SYNC_PIN, 1);
}

void Clr_Sync_Pin(){
	HAL_GPIO_WritePin (SYNC_PORT, SYNC_PIN, 0);	
}

void Set_DCDC_En_Pin(){
	HAL_GPIO_WritePin (DCDC_EN_PORT, DCDC_EN_PIN, 1);
}

void Clr_DCDC_En_Pin(){
	HAL_GPIO_WritePin (DCDC_EN_PORT, DCDC_EN_PIN, 0);
}

int  Run_ADC(){

	int g_ADCValue;

	ADC_ChannelConfTypeDef adcChannel;

    adcChannel.Channel = ADC_CHANNEL_0;
    adcChannel.Rank = 1;
    adcChannel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    adcChannel.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &adcChannel) != HAL_OK)
    {
        asm("bkpt 255");
    }

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    g_ADCValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return g_ADCValue;
}


int  ADC_Measurement(int N_ADC, int ADC_CHANNEL){

	int g_ADCValue;

	ADC_HandleTypeDef hadc;

	if(N_ADC == 1){hadc = hadc1;}
	if(N_ADC == 2){hadc = hadc2;}
	if(N_ADC == 3){hadc = hadc3;}

	ADC_ChannelConfTypeDef adcChannel;

    adcChannel.Channel = ADC_CHANNEL;
    adcChannel.Rank = 1;
    adcChannel.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    adcChannel.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc, &adcChannel) != HAL_OK)
    {
        asm("bkpt 255");
    }

    HAL_ADC_Start(&hadc);
    if (HAL_ADC_PollForConversion(&hadc, 100) == HAL_OK)
    g_ADCValue = HAL_ADC_GetValue(&hadc);
    HAL_ADC_Stop(&hadc);

    return g_ADCValue;
}

void All_Parameters_Measurement(s_ADC* stm32ADC){
	stm32ADC->I_in = ADC_Measurement(I_IN_ADC, I_IN_PIN);
	stm32ADC->U_in = ADC_Measurement(U_IN_ADC, U_IN_PIN);
	stm32ADC->I_out = ADC_Measurement(I_OUT_ADC, I_OUT_PIN);
	stm32ADC->U_out = ADC_Measurement(U_OUT_ADC, U_OUT_PIN);
	stm32ADC->t_1  = ADC_Measurement(T_1_ADC, T_1_PIN);
	stm32ADC->t_2  = ADC_Measurement(T_2_ADC, T_2_PIN);
	stm32ADC->t_3  = ADC_Measurement(T_3_ADC, T_3_PIN);
}

void TxBuffer_Send(uint8_t* Tx_Buf){
	CDC_Transmit_FS((uint8_t *)Tx_Buf, TX_DATA_SZ);
}
