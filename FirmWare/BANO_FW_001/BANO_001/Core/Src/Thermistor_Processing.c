#include <math.h>
#include "Thermistor_Processing.h"


float Estimate_R_thermistor(uint32_t ADC)
{

	float VCC = 4095.0;
	float R_up = 47000.0;
	float R_therm;
	R_therm = (float)(R_up) * (float)(ADC) / (VCC - (float)(ADC));

	return R_therm;
}

float Estimate_Temperature(uint32_t ADC)
{
	float R_therm;

	float T_K  = 273.15;
	float R0   = 47000.0;
	float B    = 4050;
	float T0   = 20.0;

	float T_msm;

	R_therm = Estimate_R_thermistor(ADC);

	T_msm = 1 / (log(R_therm / R0)  / B + 1 / (T0 + T_K)) - T_K;

	return T_msm;
}

void Give_Temperature(float T_msm, char *Temp)
{
	snprintf(Temp, 5, "%f", T_msm);
}
