
#include "stm32f10x.h"

/*
colunas:
pinos 5, 6, 7, 8 do teclado
= pinos B6, B5, B4, B3 do stm32
saídas digitais

linhas:
pinos 1, 2, 3, 4 do teclado
= pinos B12, B9, B8, B7 do stm32
entradas digitais
*/
void configura_portaB2(void){ 
	 RCC->APB2ENR |= 1<<3;    //port B clock enabled
	 GPIOB->CRH = 0x44484488; //B7 & B8 & B9 & B12 input with pull up/down,
	 GPIOB->CRL = 0x83333444; //B3 & B4 & B5 & B6 push pull max 50MHz
	 GPIOB->ODR &= ~(7<<7);   //B7 & B8 & B9 pull UP
	 GPIOB->ODR &= ~(1<<12);  //B12 pull UP
}

void atraso_debounce2(void){ //100us
	TIM2->PSC = 1;  //PRESCALER
	TIM2->ARR = 7200; //auto reload 
	TIM2->CR1 |= (1<<3); //one pulse mode
	TIM2->CR1 |= 1<<0; //counter enable
	while((TIM2->SR & 1<<0) == 0){};
	TIM2->SR &= ~(1<<0);
}

int debounce2(int linha, int coluna){
	short int bounce = 6;
	short int count = 0;
	short int key_last = 0;
	short int key_now = 0;

	GPIOB->BSRR = 1<<6; //seta todas as colunas
	GPIOB->BSRR = 1<<5;
	GPIOB->BSRR = 1<<4;
	GPIOB->BSRR = 1<<3;
	switch(coluna){ //aterra a coluna que será lida
		case 0:
		GPIOB->BSRR = 1<<(6+16);
		break;
		case 1:
		GPIOB->BSRR = 1<<(5+16);
		break;
		case 2:
		GPIOB->BSRR = 1<<(4+16);
		break;
		case 3:
		GPIOB->BSRR = 1<<(3+16);
		break;
	}

	while(count<bounce){ // debounce, diz se a linha está pressionada
		switch(linha){
			case 0:
				if((GPIOB->IDR & 1<<12) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 1:
				if((GPIOB->IDR & 1<<9) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 2:
				if((GPIOB->IDR & 1<<8) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 3:
				if((GPIOB->IDR & 1<<7) == 0) key_now = 1;
				else key_now = 0;
			break;
		}
		if (key_now==key_last) count++;
		else count = 0;
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
	for(i=0; i<5; i++){
		for(j=0; j<4; j++){
			if(debounce2(i,j)==1){
				leitura = ij_to_char2(i,j);
				return leitura;
			}
		}
	}
	return('n');
}
