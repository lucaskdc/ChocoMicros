#include "stm32f10x.h"
#include <string.h>
#include "lcd.h"
/*
pinos de dados: A0 - A7
pino de enable: C15
pino de rs:     C13
*/

inline void pulseEnable(void){
	GPIOC->BSRR = 1<<15;     // E = 1
	for(int i=0; i<50; i++); //450ns * 72MHz = 32.4, 450ns tempo m�nimo pulso enable
	GPIOC->BRR = 1<<15;      // E = 0
}

void atraso40us_lcd(void){
	TIM3->PSC = 72-1;  	//PRESCALER 72x
	TIM3->ARR = 40-1; 	//auto reload
	TIM3->CR1 |= (1<<3); //one pulse mode
	TIM3->CR1 |= 1<<0; 	//counter enable
	while((TIM3->SR & 1<<0) == 0);
	TIM3->SR &= ~(1<<0);
}

void atraso1m65s_lcd(void){
  TIM3->PSC = 72-1;  	//PRESCALER 72x
	TIM3->ARR = 1650-1; //auto reload 
	TIM3->CR1 |= (1<<3); //one pulse mode
	TIM3->CR1 |= 1<<0; 	//counter enable
	while((TIM3->SR & 1<<0) == 0);
	TIM3->SR &= ~(1<<0);
} 
void lcdComando(char data_display){
	GPIOA->ODR = (data_display & 0xFF);
	GPIOC->BRR = 1<<13;  	  //RS = 0
	pulseEnable();
	atraso40us_lcd();
}

void lcdWritechar(char data_display){
	GPIOA->ODR = (data_display & 0xFF);
	GPIOC->BSRR = 1<<13;  	 //RS = 1
	pulseEnable();
	atraso40us_lcd();
}

void clearDisplay(){
	lcdComando(0x01);
	atraso1m65s_lcd();
}

void lcdConfig(){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;  //ports A & C clock enabled
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;  //TIM3 clock enabled
	
	GPIOC->CRH &= ~0xF0F00000; //limpa conf C13 e C15
  GPIOC->CRH |= 0x30300000; //C13 & C15 push pull max 50MHz (C13 RS, C15 ENABLE)
	GPIOA->CRL &= ~0xFFFFFFFF; //limpa conf A0 at� A7
	GPIOA->CRL |= 0x33333333; //LCD DATA PINS
	
	lcdComando(0x38); //function set
	lcdComando(0x38); //function set
	lcdComando(0x0C); //display control
	lcdComando(0x06); //entry mode set
	clearDisplay();
	atraso1m65s_lcd();
}

void setCursor(unsigned int coluna, unsigned int linha){
	/*lcdComando(0x80); //coloca o cursor na linha 0
	int i = 0;
	if(linha == 1) lcdComando(0xC0); //coloca o cursor na linha 1
	while(i < coluna){      //desloca o cursor para a direita at? atingir a coluna desejada
		lcdComando(0x14);
		i++;
	}*/
	if(linha == 0)
		lcdComando(0x80+coluna%16);
	else
		lcdComando(0xC0+coluna%16);
	atraso40us_lcd(); //empirico, apenas com o atraso do comando n�o funciona.
}

void lcdWrite(char string1[16]){ //m?ximo 16 caracteres, tamanho da linha do display
	/*
	unsigned int tamanho = 0, i;
	do{ //mede o tamanho da string
		tamanho++;
	} while(string1[tamanho] != '\0'); 
	for(i = 0; i< tamanho; i++) lcdWritechar(string1[i]); //print
	*/
	for(int i = 0; i<16 && string1[i] != '\0'; i++){ //escreve enquanto index<16 e nao encontrar '\0'
		lcdWritechar(string1[i]);
	}
}

void lcdWritePos(char string1[16], unsigned int coluna, unsigned int linha){
	setCursor(coluna, linha);
	for(int i = 0; i<16 && string1[i] != '\0'; i++){ //escreve enquanto index<16 e nao encontrar '\0'
		if(coluna<16){
			coluna++;
		}else{
			coluna = 0;
			setCursor(0, ++linha);
		}
		lcdWritechar(string1[i]);
		
	}
}
