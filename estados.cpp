#include "stm32f10x.h"
#include "estados.h"
#include "lcd.h"
#include "config_perifericos.h"
#include "TECLADOMOLE.h"

extern int novoEstado;
extern int produtoConfigurado;
extern int produtoAtual;
extern int tempoUltMudancaTela;
extern int subEstado;
extern char ultTecla;

typedef void (*funcPointer)(void);
extern funcPointer funNovoEstado;

void estLoginInicial(void){
}
void estConfirma(void){
	switch (subEstado){
		case 2: if(tempoRTC() > tempoUltMudancaTela + 100)
		case 0:
			{
				clearDisplay();
				lcdWritePos("Confirmar Produção?",0,0);
				if(produtoConfigurado)
					lcdWritePos("Ao Leite",0,1);
				else
					lcdWritePos("Meio Amargo",0,1);
				tempoUltMudancaTela = tempoRTC();
				subEstado = 1;
			}
		break;
		case 1:
			if(tempoRTC() > tempoUltMudancaTela + 100){
				clearDisplay();
				lcdWritePos("Confirmar: #",0,0);
				lcdWritePos("Cancelar: D",0,1);
				tempoUltMudancaTela = tempoRTC();
				subEstado = 2;
			}
		break;
	}
	if(ultTecla == '#')
		funNovoEstado = &estInicializacao;
	else if(ultTecla == 'D')
		funNovoEstado = &estLoginInicial;
}
void estInicializacao(void){
	
}
void estProducao(void){
	
}
void estEmergencia(void){
	
}
void estLoginFechamento(void){
	
}
void estTrocaProducao(void){
	
}
void estFechamento(void){
	
}
void estRelatorio(void){
	
}