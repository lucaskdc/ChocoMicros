////////////////////////
//Faz a configura��o de alguns perif�ricos
////////////////////////
#include "stm32f10x.h"
#include "config_perifericos.h"


void configuraClock(void){
	////////////////////////////////////////
	//CLOCK, CONFIGURA CLOCK
	/* C�digo Comentado configurava para 48MHz, default � 72MHz (8MHz * 9)
	RCC->CFGR = 0; //Usa clock interno
	RCC->CR &= ~RCC_CR_PLLON; //desliga PLL
	RCC->CFGR = (RCC_CFGR_PLLMULL6); //8MHz x PLL6 = 48MHz
	RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSE); //HSE (cristal) para PLL in
	RCC->CFGR |= RCC_CFGR_PPRE1_2; //100 -> 48MHz/2 = 24MHz
	RCC->CR 	|= RCC_CR_PLLON; //Liga PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL; //Sysclock do PLL
	*/
	//////////////////////////////////
	//Liga Perif�ricos
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN //Liga ADC1
							 | RCC_APB2ENR_TIM1EN  //Liga Timer1
							 | RCC_APB2ENR_IOPAEN  //Liga PortA
							 | RCC_APB2ENR_IOPBEN  //Liga PortB
							 | RCC_APB2ENR_IOPCEN; //Liga PortC
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN //Liga Timer2
							 | RCC_APB1ENR_TIM3EN; //Liga Timer3
	configA8MCO(); // usado para verificar PLL/2, usa o pino A8
}

void configGPIOs(void){
	//display lcd:
  GPIOC->CRH &= ~0xF0F00000; //limpa conf C13 e C15
  GPIOC->CRH |= 0x30300000; //C13 & C15 push pull max 50MHz (C13 RS, C15 ENABLE)
	GPIOA->CRL &= ~0xFFFFFFFF; //limpa conf A0 at� A7
	GPIOA->CRL |= 0x33333333; //LCD DATA PINS
	
	//teclado:
	GPIOB->CRH = (GPIOB->CRH & 0xFFFF0000) | 0x00003388; //B8 & B9 input with pull up/down, B10 e B11 push pull
	GPIOB->CRL = (GPIOB->CRL & 0x00000FF0) | 0x88344003; //B3 & B4 input desativado, B5 e B0 push pull max 50MHz, B6 & B7 input pull up/down
	GPIOA->CRH = (GPIOA->CRH & 0x0FFFFFFF) | 0x4<<28; //A15 input desativado
	GPIOB->BSRR = 0xF << 6;   //B9, B8, B7, B6 linhas pull up
	
	//SENSORES DIGITAIS
	//B13 entrada reservatorio vazio
	//B14 entrada reservatorio cheio
	GPIOB->CRH = (GPIOB->CRH & 0xF00FFFFF) | 0x04400000; //B13 & B14 input floating

	//ATUADORES DIGITAIS
	//A10 pist�o de sa�da de chocolate
	//A11 esteira
	//A12 v�lvula movimenta��o de chocolate
	GPIOA->BSRR = (7<<10); // HIGH A10, A11 e A12, esteira desligada
	GPIOA->CRH = (GPIOA->CRH & 0xFFF000FF) | 0x00077700; //open drain A10 & A11 & A12 OUTPUT push pull max 50MHz
	
	
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
	int tempo = (RTC->CNTH << 16) | (RTC->CNTL);
	return tempo;
}

void configADC(void){
	///////////////////////////////////
	//Configura ADC
	RCC->CFGR |= RCC_CFGR_ADCPRE_1; //ADCPRE=10 ; 72MHz/6 = 12MHz que eh menor que 14MHz
	ADC1->CR1 |= ADC_CR1_DISCEN; //modo descontinuo
	ADC1->CR1 |= ADC_CR1_EOCIE; //ativa interrup��o fim de convers�o
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
	TIM2->CCER |= TIM_CCER_CC2E; //ativa compara��o
	TIM2->PSC = 36000-1; //prescaller /36000 -> 2kHz
	TIM2->ARR = 200-1; //Auto-reload -> f = 2kHz/200 = 10Hz (taxa de amostragem)
	TIM2->CCR2 = 50;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void configA8MCO(){ //configura pino A8 para sa�da MCO (para fins de teste de clock)
	RCC->CFGR |= RCC_CFGR_MCO_PLL; //
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; //PLL/2 out MCO
	GPIOA->CRH &= ~0xF;
	GPIOA->CRH |= 0x8; //Modo alternativo (sa�da clock) pushpull A8
	GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ; //50MHz
}

void configTIM4(){
	
}
