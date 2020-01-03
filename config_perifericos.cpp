////////////////////////
//Faz a configuração de alguns periféricos
////////////////////////
#include "stm32f10x.h"
#include "config_perifericos.h"

void configuraClock(void){
	////////////////////////////////////////
	//CLOCK, CONFIGURA CLOCK
	/* Código Comentado configurava para 48MHz, default é 72MHz (8MHz * 9)
	RCC->CFGR = 0; //Usa clock interno
	RCC->CR &= ~RCC_CR_PLLON; //desliga PLL
	RCC->CFGR = (RCC_CFGR_PLLMULL6); //8MHz x PLL6 = 48MHz
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSE); //HSE (cristal) para PLL in
	RCC->CFGR |= RCC_CFGR_PPRE1_2; //100 -> 48MHz/2 = 24MHz
	RCC->CR 	|= RCC_CR_PLLON; //Liga PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL; //Sysclock do PLL
	*/
	//////////////////////////////////
	//Liga Periféricos
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN //Liga ADC1
							 | RCC_APB2ENR_TIM1EN //Liga Timer1
							 | RCC_APB2ENR_IOPAEN //Liga PortA
							 | RCC_APB2ENR_IOPBEN	//Liga PortB
							 | RCC_APB2ENR_IOPCEN; //Liga PortC
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //Liga Timer2
	configA8MCO(); // usado para verificar PLL/2, usa o pino A8
}

void configRTC(void){
	////////////////////////////////////
	//Configura RTC
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
	PWR->CR |= PWR_CR_DBP;	
	RCC->BDCR |= RCC_BDCR_RTCSEL_HSE; //usa clock HSE (cristal 8MHz)
	RCC->BDCR |= RCC_BDCR_RTCEN; //ativa RTC
	RTC->CRL |= RTC_CRL_CNF; //entra modo de conf
	RTC->PRLH = 0;
	RTC->PRLL = 625; //8MHz HSE /128 /625 = 100Hz
	RTC->CNTH = 0;
	RTC->CNTL = 0;
	RTC->CRL &= ~RTC_CRL_CNF; //configura e liga
}

int tempoRTC(void){
	return (RTC->CNTH << 16) | (RTC->CNTL);
}

void configADC(void){
	///////////////////////////////////
	//Configura ADC
	RCC->CFGR |= RCC_CFGR_ADCPRE_1; //ADCPRE=10 ; 72MHz/6 = 12MHz que eh menor que 14MHz
	ADC1->CR1 |= ADC_CR1_DISCEN; //modo descontinuo
	ADC1->CR1 |= ADC_CR1_EOCIE; //ativa interrupção fim de conversão
	NVIC->ISER[0] = NVIC_ISER_SETENA_18; //ativa interrupt
	GPIOB->CRL &= ~0xF0; //analog input B1 (IN9)
	ADC1->SQR3 = 9; //primeira conversao canal 9
	ADC1->SMPR2 = ADC_SMPR2_SMP9; //111 -> max tempo de amostragem
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->CR2 |= ADC_CR2_CAL;
	while(ADC1->CR2 & ADC_CR2_CAL);
	ADC1->CR2 |= ADC_CR2_EXTTRIG
						| (0x3<<17); //EXTSEL TIMER2CC2 event
						
	///////////////////////////////////
	//Configura Timer2 (tempo de amostragem ADC)
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_2|TIM_CCMR1_OC2M_1; //110: PWM mode 1
	TIM2->CCER |= TIM_CCER_CC2E; //ativa comparação
	TIM2->PSC = 36000-1; //prescaller /36000 -> 2kHz
	TIM2->ARR = 200-1; //Auto-reload -> f = 2kHz/200 = 10Hz (taxa de amostragem)
	TIM2->CCR2 = 50;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void configA8MCO(){ //configura pino A8 para saída MCO (para fins de teste de clock)
	RCC->CFGR |= RCC_CFGR_MCO_PLL; //
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; //PLL/2 out MCO
	GPIOA->CRH &= ~0xF;
	GPIOA->CRH |= 0x8; //Modo alternativo (saída clock) pushpull A8
	GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ; //50MHz
}

void configTIM4(){
	
}
