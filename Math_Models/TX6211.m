
U_in = 12;
U_out = 36;

C1 = 22 * 10^-12

T_D = 61 * 10^-9;

T_OFF = 0.51 * 150000 * (C1 + 7.3 * 10^-12) + T_D

Fs = U_in / (U_out * T_OFF)


VDD = 5.5;
I_VDD = 2 * 10^-3;
R_VDD = (U_in - VDD) / I_VDD