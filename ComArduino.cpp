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
