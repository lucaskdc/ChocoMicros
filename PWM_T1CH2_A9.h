#ifndef PWM_T1CH2_A9_h
#define PWM_T1CH2_A9_h
#define CiclosInicial 10000
#define CCR2Inicial 0
class PWM{
	public:
	void setComp(int compara);
	void setCycles(int timerClocksCycles);
	void setDutyCycle(float percentagem);
	PWM();
};

#endif