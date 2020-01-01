#include "stm32f10x.h"
#include "PWM_T1CH2_A9.h"
#include "lcd.h"
#include "TECLADOMOLE.h"
#include <string.h>

PWM atuador;
float e[2] = {0};
float referencia = 0.50; //50%
const float Kp=2, Kd=0.5;


extern "C" void ADC1_2_IRQHandler(){
	e[1] = e[0];
	e[0] = (float)ADC1->DR/(1<<12) - referencia; //normalizado 0 a 1
	float u = Kp*e[0] + Kd*(e[0]-e[1]);
	atuador.setDutyCycle(u*100);
}

void float2str(char text[6], float val){
	text[0]=(int)val%100/10+0x30;
	text[1]=(int)val%10+0x30;
	text[2]='.';
	text[3]=(int)(val*10)%10+0x30;
	text[4]=(int)(val*100)%10+0x30;
	text[5]=0;
}



int main(){
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
	RCC->CFGR |= RCC_CFGR_MCO_PLL; //
	RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; //PLL/2 out MCO
	
	//////////////////////////////////
	//Liga Periféricos
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN //Liga ADC1
							 | RCC_APB2ENR_TIM1EN //Liga Timer1
							 | RCC_APB2ENR_IOPAEN //Liga PortA
							 | RCC_APB2ENR_IOPBEN	//Liga PortB
							 | RCC_APB2ENR_IOPCEN; //Liga PortC
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; //Liga Timer2
	
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

	////////////////////////////////////
	//testa classe PWM, atuador é var global
	atuador = PWM();	//inicializa PWM
	atuador.setDutyCycle(50.5); 
	atuador.disableOutput(); //testando funcao
	atuador.enableOutput();
	

	
	
	////////////////////////////
	//testes de GPIOs A8 é saída de clock sysclock (para fins de verificação)
	GPIOA->CRH &= ~0xF;
	GPIOA->CRH |= 0x00000008; //Modo alternativo (saída clock) pushpull A8
	GPIOA->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_MODE8_0 ; //50MHz
	GPIOC->CRH &= ~0xF0F00000; //limpa config C13 e C15
	GPIOC->CRH |= GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0; //50MHz
	GPIOC->CRH |= GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0; //50MHz
	GPIOB->CRL &= ~0xF;
	GPIOB->CRL |= 0x3;
	
	
	lcdConfig();
	lcdConfig();
	lcdWrite("OLA");

	configura_portaB2();
//	int i=0;
	char vect[16];
	for(;;){
		char var = le_teclado2();
		if(var)
			lcdWritechar(var);
		while((GPIOB->IDR & 0xF<<6) != 0xF<<6); //trava enquanto botao estiver pressionado
		float2str(vect, atuador.getDutyCycle());
		lcdWritePos(vect,0,1);
	}
}

