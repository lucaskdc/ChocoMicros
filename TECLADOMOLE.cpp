
#include "stm32f10x.h"


//pinos B4, B3, A15 queimados
/*
colunas:
pinos 5, 6, 7, 8 do teclado
= pinos B5, B0, B10, B11 do stm32f103
sa�das digitais

linhas:
pinos 1, 2, 3, 4 do teclado
= pinos B9, B8, B7, B6 do stm32f103
entradas digitais
*/


#define SETCOLUNA0() GPIOB->BSRR = 1<<5
#define SETCOLUNA1() GPIOB->BSRR = 1<<0
#define SETCOLUNA2() GPIOB->BSRR = 1<<10
#define SETCOLUNA3() GPIOB->BSRR = 1<<11

#define RESETCOLUNA0() GPIOB->BRR = 1<<5
#define RESETCOLUNA1() GPIOB->BRR = 1<<0
#define RESETCOLUNA2() GPIOB->BRR = 1<<10
#define RESETCOLUNA3() GPIOB->BRR = 1<<11


void atraso_debounce2(void){ //100us
	TIM3->PSC = 0;  //PRESCALER
	TIM3->ARR = 4800-1; //auto reload -> 4800/48MHz = 100us
	TIM3->CR1 |= (1<<3); //one pulse mode
	TIM3->CR1 |= 1<<0; //counter enable
	while((TIM3->SR & 1<<0) == 0){};
	TIM3->SR &= ~(1<<0);
}

int debounce2(int linha, int coluna){
	const int bounce = 6;
	int count = 0;
	int key_last = 0;
	int key_now = 0;

	SETCOLUNA0();
	SETCOLUNA1();
	SETCOLUNA2();
	SETCOLUNA3();
	switch(coluna){ //aterra a coluna que ser� lida
		case 0:
		//GPIOB->BRR = 1<<5; //col 0 B5
		RESETCOLUNA0();
		break;
		case 1:
		//GPIOB->BRR = 1<<4; //col 1 B4
		RESETCOLUNA1();
		break;
		case 2:
		//GPIOB->BRR = 1<<3; //col 2 B3
		RESETCOLUNA2();
		break;
		case 3:
		//GPIOA->BRR = 1<<15; //col 3 A15
		RESETCOLUNA3();
		break;
	}

	while(count<bounce){ // debounce, diz se a linha est� pressionada
		switch(linha){
			case 0:
				if((GPIOB->IDR & 1<<9) == 0) key_now = 1; //lin 0 B9
				else key_now = 0;
			break;
			case 1:
				if((GPIOB->IDR & 1<<8) == 0) key_now = 1; //lin 1 B8
				else key_now = 0;
			break;
			case 2:
				if((GPIOB->IDR & 1<<7) == 0) key_now = 1; //lin 2 B7
				else key_now = 0;
			break;
			case 3:
				if((GPIOB->IDR & 1<<6) == 0) key_now = 1; //lin 3 B6
				else key_now = 0;
			break;
		}
		if (key_now==key_last)
			count++;
		else 
			count = 0;
		key_last = key_now;
		atraso_debounce2();
	}
	return (key_now);
}

char ij_to_char2(int linha, int coluna){
	char leitura;
	switch(linha){
		case 0:
		if(coluna==0) leitura = '1';
		if(coluna==1) leitura = '2';
		if(coluna==2) leitura = '3';
		if(coluna==3) leitura = 'A';
		break;
		case 1:
		if(coluna==0) leitura = '4';
		if(coluna==1) leitura = '5';
		if(coluna==2) leitura = '6';
		if(coluna==3) leitura = 'B';
		break;
		case 2:
		if(coluna==0) leitura = '7';
		if(coluna==1) leitura = '8';
		if(coluna==2) leitura = '9';
		if(coluna==3) leitura = 'C';
		break;
		case 3:
		if(coluna==0) leitura = '*';
		if(coluna==1) leitura = '0';
		if(coluna==2) leitura = '#';
		if(coluna==3) leitura = 'D';
		break;
		}
	return leitura;
}

char le_teclado2(){
	int i; //linha
	int j; //coluna
	char leitura;
	for(i=0; i<4; i++){
		for(j=0; j<4; j++){
			if(debounce2(i,j)==1){
				leitura = ij_to_char2(i,j);
				return leitura;
			}
		}
	}
	return 0;
}
