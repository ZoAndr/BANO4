#ifndef THERMISTOR_PROCESSING_H
#define THERMISTOR_PROCESSING_H

#include <stdint.h>

float Estimate_R_thermistor(uint32_t ADC);
float Estimate_Temperature(uint32_t ADC);
void Give_Temperature(float T_msm, char *Temp);

#endif
