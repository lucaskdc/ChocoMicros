///////////////////////////
//Biblioteca para comunicação com interface para Computador (Arduino)
////////////////////////////
#include "stm32f10x.h"
#include "ComArduino.h"

inline int reservatorioCheio(){
	return GPIOB->IDR & (1<<14) ? 1 : 0; //se B14 HIGH, retorna 1, caso contrário 0
}
inline int reservatorioVazio(){
	return GPIOB->IDR & (1<<13) ? 1 : 0; //se B13 HIGH, retorna 1, caso contrário 0
}

inline int statusPistao(void){ //1 ativo, 0 desativado
	return GPIOA->ODR & (1<<10) ? 0 : 1; //retorna 1 se A10 está em LOW e 0 se está em HIGH
}
inline int statusEsteira(void){ //1 ligada, 0 desligada
	return GPIOA->ODR & (1<<11) ? 0 : 1; //retorna 1 se A11 está em LOW e 0 se está em HIGH
}
inline int statusValvula(void){ //1 aberta, 0 fechada
	return GPIOA->ODR & (1<<12) ? 0 : 1; //retorna 1 se A12 está em LOW e 0 se está em HIGH
}

inline void ativaPistao(void){
	GPIOA->BRR = (1<<10); //ativa pistão com 0 em A10
}
inline void desativaPistao(void){
	GPIOA->BSRR = (1<<10); //desativa pistão com 1 em A10
}
inline void ligaEsteira(void){
	GPIOA->BRR = (1<<11); //liga esteira com 0 em A11
}
inline void desligaEsteira(void){
	GPIOA->BSRR = (1<<11); //desliga pistão com 1 em A11
}
inline void abreValvula(void){
	GPIOA->BRR = (1<<12); //abre válvula com 0 em A12
}
inline void fechaValvula(void){
	GPIOA->BSRR = (1<<12); //fecha válvula com 1 em A12
}
