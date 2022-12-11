clc
clear all

U_in = 12;
U_out = 36;
I_LED = 1;

V_D = 0.95;

L = 3.3 * 10^(-6);
Fsw = 1e6;

disp(['Parameters: U_in = ' sprintf('%.3f',U_in) 'V, U_out = ' sprintf('%.3f',U_out) 'V'])
disp(['            I_LED = ' sprintf('%.3f',I_LED) 'A, L = ' sprintf('%.1f',L * 1e6) 'uH'])

% Soft start parameters
R_SS = 500 * 1000;
V_BP = 8.0;
V_SS = 0.7;
V_FB_REF = 0.7;

t_ss = 0.1;

C_ss = t_ss / (    R_SS * log(  (V_BP - V_SS) / (V_BP - (V_SS + V_FB_REF) ) )   );

disp(['Soft start: tss = ' sprintf('%.3f',t_ss * 1e3) 'ms, Css = ' sprintf('%.2f',C_ss * 1e9), ' nF']);

% Duty cycle
D_CCM = (1 - ( U_in / (U_out - V_D) ));

D_DCM = (  2 * (U_out + V_D) * I_LED * L * Fsw  ) /  (U_in^2);

I_LED_crt = (U_out + V_D - U_in) * (U_in^2) / ( 2 * (U_out + V_D)^2 * Fsw * L);

if I_LED > I_LED_crt
    D = D_CCM;
    disp(['Duty cycle: CCM, D = ' sprintf('%.3f', D)]);
else
    D = D_DCM;
    disp(['Duty cycle: DCM, D = ' sprintf('%.3f', D)]);
end

% Oscillator
Fsw = 1e3;
C_t = 82; %68 - 120 pF, R_t must be 100k - 1MOhm

R_t = (5.8e-8*Fsw * C_t   +   8e-10 * (Fsw^2)   +   1.4e-7 * Fsw   -   1.5e-4   +   1.7e-6 * C_t   -   4e-9 * (C_t^2))^-1;

disp(['Oscillator:  R_t = ' sprintf('%.2f',R_t) 'kOhm, C_t = ' sprintf('%.2f',C_t) 'pF, Fsw = ' sprintf('%.3f',Fsw) 'kHz']);


% Current mode control and overcurrent protection
Fsw = Fsw * 1e3;
V_ISNS = 0.150; % V min .120, max .180

if I_LED > I_LED_crt
    D = D_CCM;
    R_ISNS = V_ISNS / (  (I_LED / (1 - D))  +  (D * U_in / (2 * Fsw * L))  );
    I_RIPPLE = D * U_in / (Fsw * L);
    disp(['CCM. R_ISNS = ' sprintf('%.4f', R_ISNS) 'Ohm. I_RIPPLE = ' sprintf('%.4f', I_RIPPLE)]);
else
    D = D_DCM;
    I_LED_oc = 5; % Overcurrent threshold current
    R_ISNS = (Fsw * L * V_ISNS) / (2 * L * Fsw * I_LED_oc * (U_out + V_D - U_in))^2;
    disp(['CCM. R_ISNS = ' sprintf('%.4f', R_ISNS) 'Ohm. Overcurrent threshold current = ' sprintf('%.4f', I_LED_oc)]);
end
I_max = V_ISNS / R_ISNS

VDD = U_in;
R_ISNS_max = VDD * L * Fsw / (60 * (U_out + V_D - U_in))
I_max = V_ISNS / R_ISNS_max