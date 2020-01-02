///////////////////////////
//Biblioteca para comunica��o com interface para Computador (Arduino)
////////////////////////////
#include "stm32f10x.h"
#include "ComArduino.h"

inline int reservatorioCheio(){
	return GPIOB->IDR & (1<<14) ? 1 : 0; //se B14 HIGH, retorna 1, caso contr�rio 0
}
inline int reservatorioVazio(){
	return GPIOB->IDR & (1<<13) ? 1 : 0; //se B13 HIGH, retorna 1, caso contr�rio 0
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
inline void fechaValvula(void){
	GPIOA->BSRR = (1<<12); //fecha v�lvula com 1 em A12
}
