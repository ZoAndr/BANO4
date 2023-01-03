struct s_HW_ADC{
	int A1[8];
	int A2[8];
	int A3[8];
};

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

struct s_System {
	int Seeds_cntr;
	int InPeriod_cntr;
	int Periods_cntr;
	int ADC_cntr;
	int Strobe;
	int DCDC_On;
};


void Convert_ADC_to_Tx_format(struct s_ADC *ADC, struct s_Tx_Parameters *Tx_Prms);
void Create_Tx_Buffer(struct s_Tx_Parameters* Tx_Prms, char TX_Buf[TX_BUF_SZ]);
int On_PC_Create_Rx_Parcel(struct s_Rx_Parameters* Rx_Parameters, char RX_Buf[RX_BUF_SZ]);
int Process_Rx_Parcel(struct s_Rx_Parameters* Rx_Parameters, char RX_Buf[RX_BUF_SZ]);
void Initialize_System(struct s_System* System);
void Timer_100us(struct s_Rx_Parameters* Rx_Parameters, 
	             char RX_Buf[RX_BUF_SZ], 
	             struct s_System *System,
	             struct s_ADC* ADC,
	             struct s_Tx_Parameters* Tx_Parameters,
	             char TX_Buf[TX_BUF_SZ]
	);