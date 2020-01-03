///////////////////////////
//Biblioteca para comunica��o com interface para Computador (Arduino)
////////////////////////////
#include "stm32f10x.h"
#include "ComArduino.h"

extern int tempofechado1min;
void fechaValvula(void){
	if(!(GPIOA->ODR & (1<<12))) //se valvula tava aberta (LOW), fecha nesse momento e conta tempo
		tempofechado1min=tempoRTC();
	GPIOA->BSRR = (1<<12); //fecha v�lvula com 1 em A12
}
