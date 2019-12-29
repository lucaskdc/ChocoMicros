#include "stm32f10x.h"
#include <string.h>

/*
pinos de dados: A0 - A7
pino de enable: C15
pino de rs:     C13
*/


void atraso_40us_lcd(void){
	TIM2->PSC = 1;  //PRESCALER
	TIM2->ARR = 2880; //auto reload 
	TIM2->CR1 |= (1<<3); //one pulse mode
	TIM2->CR1 |= 1<<0; //counter enable
	while((TIM2->SR & 1<<0) == 0){};
	TIM2->SR &= ~(1<<0);
}

void atraso_1m65s_lcd(void){
  TIM2->PSC = 4;  //PRESCALER
	TIM2->ARR = 29700; //auto reload 
	TIM2->CR1 |= (1<<3); //one pulse mode
	TIM2->CR1 |= 1<<0; //counter enable
	while((TIM2->SR & 1<<0) == 0){};
	TIM2->SR &= ~(1<<0);
}
 
void lcd_comando(char data_display){
	GPIOA->ODR = (data_display & 0xFF);
	GPIOC->BSRR = 1<<(13+16);  	  //RS = 0
  GPIOC->BSRR = 1<<15;          // E = 1
	GPIOC->BSRR = 1<<(15+16);     // E = 0
	atraso_40us_lcd();
}

void lcdWritechar(char data_display){
	GPIOA->ODR = (data_display & 0xFF);
	GPIOC->BSRR = 1<<13;  	      //RS = 1
	GPIOC->BSRR = 1<<15;          // E = 1
	GPIOC->BSRR = 1<<(15+16);     // E = 0
	atraso_40us_lcd();
}

void clear_display(){
	lcd_comando(0x01);
	atraso_1m65s_lcd();
}

void lcd_config(){
	RCC->APB2ENR |= 5<<2;  //ports A & C clock enabled
	RCC->APB1ENR |= 1<<0;  //TIM2 clock enabled
	
  GPIOC->CRH = 0x34344444; //C13 & C15 push pull max 50MHz (C13 RS, C15 ENABLE)
	GPIOA->CRL = 0x33333333; //LCD DATA PINS
	
	lcd_comando(0x38); //function set
	lcd_comando(0x38); //function set
	lcd_comando(0x0C); //display control
	lcd_comando(0x06); //entry mode set
	clear_display();
	atraso_1m65s_lcd();
}

void setCursor(unsigned int coluna, unsigned int linha){
	lcd_comando(0x80); //coloca o cursor na linha 0
	int i = 0;
	if(linha == 1) lcd_comando(0xC0); //coloca o cursor na linha 1
	while(i < coluna){      //desloca o cursor para a direita at? atingir a coluna desejada
		lcd_comando(0x14);
		i++;
	}
}

void lcdWrite(char string1[16]){ //m?ximo 16 caracteres, tamanho da linha do display
	unsigned int tamanho = 0, i;
	do{ //mede o tamanho da string
		tamanho++;
	} while(string1[tamanho] != '\0'); 
	for(i = 0; i< tamanho; i++) lcdWritechar(string1[i]); //print
}
