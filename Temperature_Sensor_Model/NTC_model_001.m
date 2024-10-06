T1 = -40: 125;
T0 = 25;
B = 4050;
R0 = 47000;

TK = 273.15;

% The equation for NTC thermistor resistance calculation from temperature (T1)
R1 = R0 .* exp(B * (1 ./ (T1 + TK) - 1 ./ (T0 + TK)));

figure(10);
semilogy(T1, R1 / 1e3);
grid on

% The equation for temperature estimation from NTC thermistor resistance
T_est = 1 ./ (log(R1 ./ R0) ./ B + 1 / (T0 + TK)) - TK;


R_47k = 42000;
U_VCC = 4095;
U_ADC = 1967;

% Calculation of NTC thermistor resistance from simple 2R-divider with upper arm 47 kOhm
% Voltages are in direct ADC scales
R_therm  = R_47k * U_ADC / (U_VCC - U_ADC);

% Equation for temperature estimation from measured NTC thermistor resistance
T_msm = 1 ./ (log(R_therm ./ R0) ./ B + 1 / (T0 + TK)) - TK;
