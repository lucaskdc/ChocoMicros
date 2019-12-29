#ifndef PWM_T1CH2_A9_h
#define PWM_T1CH2_A9_h
#define CiclosInicial 10000 //Define numero de ciclos. freq_PWM = APB
#define CCR2Inicial 0
class PWM{
	public:
	void setComp(int compara); //define TIM1CCR2 com saturação em 0 e ARR+1
	void setCycles(int timerClockCycles); //define ARR como timerClocksCycles -1
	void setDutyCycle(float percentagem); //define dutycycle usando percentagem
	float getDutyCycle(void); //retorna dutycycle
	void disableOutput(void); //desativa atuador
	void enableOutput(void); //reativa atuador
	PWM();
};

#endif
