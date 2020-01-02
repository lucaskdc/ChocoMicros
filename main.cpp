#include "stm32f10x.h"
#include "PWM_T1CH2_A9.h"
#include "lcd.h"
#include "TECLADOMOLE.h"
#include <string.h>
#include "config_perifericos.h"
#include "auxiliares.h"

PWM aquecedor;
volatile float e[2] = {0};
volatile float referencia = 0.50; //50%
const float Kp=2, Kd=0.5;

extern "C" void ADC1_2_IRQHandler(){
	e[1] = e[0];
	e[0] = (float)ADC1->DR/(1<<12) - referencia; //normalizado 0 a 1
	float u = Kp*e[0] + Kd*(e[0]-e[1]);
	aquecedor.setDutyCycle(u*100);
}

int main(){
	configuraClock();
	configRTC();
	configADC();
	
	aquecedor = PWM();	//inicializa PWM, aquecedor é var global
	
	lcdConfig();
	lcdWrite("OLA");

	configura_portaB_teclado2();
//	int i=0;
	char vect[16];
	for(;;){
		char var = le_teclado2();
		if(var)
			lcdWritechar(var);
		while((GPIOB->IDR & 0xF<<6) != 0xF<<6); //trava enquanto botao estiver pressionado
		
		porcentagemFloat2Str(vect, aquecedor.getDutyCycle());
		lcdWritePos(vect,10,1);
	}
}

