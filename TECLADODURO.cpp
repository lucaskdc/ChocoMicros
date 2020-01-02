#include "stm32f10x.h"

/*
colunas:
0,1,2,3 = B9, B8, B7, B6
linhas:
0,1,2,3,4 = B5, B4, B3, B15, B12
*/

void configura_portaB(void){ 
	 RCC->APB2ENR |= 1<<3;    //port B clock enabled
	 GPIOB->CRH = 0x84484433; //B8 & B9   push pull max 50MHz, B12 & B15 input
	 GPIOB->CRL = 0x33888434; //B3 & B4 & B5 input with pull up/down, B6 & B7 push pull
	 GPIOB->ODR &= ~(1<<12);  //B12 pull UP
	 GPIOB->ODR &= ~(1<<15);  //B15 pull UP
	 GPIOB->ODR &= ~(7<<3);   //B3 & B4 & B5 pull UP
	//GPIOB->BSRR = 1<<9;
}

void atraso_debounce(void){ //100us
	TIM3->PSC = 1;  //PRESCALER
	TIM3->ARR = 7200; //auto reload 
	TIM3->CR1 |= (1<<3); //one pulse mode
	TIM3->CR1 |= 1<<0; //counter enable
	while((TIM3->SR & 1<<0) == 0){};
	TIM3->SR &= ~(1<<0);
}

int debounce(int linha, int coluna){
	short int bounce = 6;
	short int count = 0;
	short int key_last = 0;
	short int key_now = 0;

	GPIOB->BSRR = 1<<6; //seta todas as colunas
	GPIOB->BSRR = 1<<7;
	GPIOB->BSRR = 1<<8;
	GPIOB->BSRR = 1<<9;
	switch(coluna){ //aterra a coluna que será lida
		case 0:
		GPIOB->BSRR = 1<<(9+16);
		break;
		case 1:
		GPIOB->BSRR = 1<<(8+16);
		break;
		case 2:
		GPIOB->BSRR = 1<<(7+16);
		break;
		case 3:
		GPIOB->BSRR = 1<<(6+16);
		break;
	}

	while(count<bounce){ // debounce, diz se a linha está pressionada
		switch(linha){
			case 0:
				if((GPIOB->IDR & 1<<5) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 1:
				if((GPIOB->IDR & 1<<4) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 2:
				if((GPIOB->IDR & 1<<3) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 3:
				if((GPIOB->IDR & 1<<15) == 0) key_now = 1;
				else key_now = 0;
			break;
			case 4:
				if((GPIOB->IDR & 1<<12) == 0) key_now = 1;
				else key_now = 0;
			break;
		}
		if (key_now==key_last) count++;
		else count = 0;
		key_last = key_now;
		atraso_debounce();
	}
	return (key_now);
}

char ij_to_char(int linha, int coluna){
	char leitura;
	switch(linha){
		case 0:
		if(coluna==0) leitura = 'n';
		if(coluna==1) leitura = '0';
		if(coluna==2) leitura = '*';
		if(coluna==3) leitura = 'n';
		break;
		case 1:
		if(coluna==0) leitura = '1';
		if(coluna==1) leitura = '2';
		if(coluna==2) leitura = '3';
		if(coluna==3) leitura = 'E';
		break;
		case 2:
		if(coluna==0) leitura = '4';
		if(coluna==1) leitura = '5';
		if(coluna==2) leitura = '6';
		if(coluna==3) leitura = 'D';
		break;
		case 3:
		if(coluna==0) leitura = '7';
		if(coluna==1) leitura = '8';
		if(coluna==2) leitura = '9';
		if(coluna==3) leitura = 'U';
		break;
		case 4:
		if(coluna==0) leitura = 'L';
		if(coluna==1) leitura = 'R';
		if(coluna==2) leitura = 'X';
		if(coluna==3) leitura = 'n';
		break;
		}
	return leitura;
}

char le_teclado(){
	int i; //linha
	int j; //coluna
	char leitura;
	for(i=0; i<5; i++){
		for(j=0; j<4; j++){
			if(debounce(i,j)==1){
				leitura = ij_to_char(i,j);
				return leitura;
			}
		}
	}
	return('n');
}
