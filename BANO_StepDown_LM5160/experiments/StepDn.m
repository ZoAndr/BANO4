
clear all; close all; clc;

L = 6.8 * 1e-6;

fs = 750e3;

n = 0.85;

U_i = 13: 3: 31;
U_o = 9.6;

dt = 1 / fs;
dt = 1400e-9;

D = 1 ./ ((U_i) ./ U_o)

t_on = dt .* D / n %* 1e6

dI = (U_i - U_o) .* t_on / L