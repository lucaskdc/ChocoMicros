#include "PWM_T1CH2_A9.h"
#include "stm32f10x.h"

PWM::PWM(){
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1; //110: PWM mode 1
	TIM1->CCER |= TIM_CCER_CC2E; //ativa comparação
	TIM1->ARR = CiclosInicial-1; //Auto-reload
	TIM1->CCR2 = CCR2Inicial;
	
	GPIOA->CRH &= ~(0xF << 4); //limpa configuração A9
	GPIOA->CRH |= (0x8 << 4); //0100 saída alternativa (Ch2 Timer1) em pushpull
	GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0 ; //50MHz
	
	TIM1->BDTR |= TIM_BDTR_MOE; //ativa saída do timer (timer ch2 -> pino A9)
	TIM1->CR1 |= TIM_CR1_CEN; //ativa contagem timer1
}

void PWM::setComp(int compara){
	if(compara <= 0)
		compara = 0;
	if(compara >= TIM1->ARR + 1)
		compara = TIM1->ARR + 1;
	
	TIM1->CCR2 = compara;
}

void PWM::setCycles(int timerClockCycles){
	TIM1->ARR = timerClockCycles - 1;
}

void PWM::setDutyCycle(float percentagem){
	TIM1->CCR2 = (int)(percentagem/100.0*(TIM1->ARR+1));
}