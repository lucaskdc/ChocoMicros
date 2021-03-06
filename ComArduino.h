///////////////////////////
//Biblioteca para comunica��o com interface para Computador (Arduino)
////////////////////////////

#ifndef COM_ARDUINO_H
#define COM_ARDUINO_H
	#include "stm32f10x.h"
	#include "config_perifericos.h" //funcao do RTC
	
	inline int reservatorioCheio(void);
	inline int reservatorioVazio(void);
	
	inline int statusPistao(void); //1 ativo, 0 desativado
	inline int statusEsteira(void); //1 ligada, 0 desligada
	inline int statusValvula(void); //1 aberta, 0 fechada
	
	inline void ativaPistao(void);
	inline void desativaPistao(void);
	inline void ligaEsteira(void);
	inline void desligaEsteira(void);
	inline void abreValvula(void);
	void fechaValvula(void);
	
	inline int reservatorioCheio(){
		return GPIOB->IDR & (1<<14) ? 1 : 0; //se B14 HIGH, retorna 1, caso contr�rio 0
	}
	inline int reservatorioVazio(){
		return GPIOB->IDR & (1<<13) ? 0 : 1; //se B13 low, retorna 1, caso contr�rio 0
	}

	inline int statusPistao(void){ //1 ativo, 0 desativado
		return GPIOA->ODR & (1<<10) ? 0 : 1; //retorna 1 se A10 est� em LOW e 0 se est� em HIGH
	}
	inline int statusEsteira(void){ //1 ligada, 0 desligada
		return GPIOA->ODR & (1<<11) ? 0 : 1; //retorna 1 se A11 est� em LOW e 0 se est� em HIGH
	}
	inline int statusValvula(void){ //1 aberta, 0 fechada
		return GPIOA->ODR & (1<<12) ? 0 : 1; //retorna 1 se A12 est� em LOW e 0 se est� em HIGH
	}

	inline void ativaPistao(void){
		GPIOA->BRR = (1<<10); //ativa pist�o com 0 em A10
	}
	inline void desativaPistao(void){
		GPIOA->BSRR = (1<<10); //desativa pist�o com 1 em A10
	}
	inline void ligaEsteira(void){
		GPIOA->BRR = (1<<11); //liga esteira com 0 em A11
	}
	inline void desligaEsteira(void){
		GPIOA->BSRR = (1<<11); //desliga pist�o com 1 em A11
	}
	inline void abreValvula(void){
		GPIOA->BRR = (1<<12); //abre v�lvula com 0 em A12
	}
	
#endif
