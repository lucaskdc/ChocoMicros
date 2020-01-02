#include "stm32f10x.h"
#include "estados.h"
#include "lcd.h"
#include "config_perifericos.h"
#include "TECLADOMOLE.h"
#include "PWM_T1CH2_A9.h"
#include "auxiliares.h"
#include "ComArduino.h"



extern int novoEstado;
extern int produtoConfigurado;
extern int produtoAtual;
extern int tempoUltMudancaTela;
extern int subEstado;
extern char ultTecla;
extern PWM aquecedor;
extern float e[2];
extern float referencia;

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
	switch(subEstado){
		case 0:
			clearDisplay();
			lcdWritePos("Inic. Producao",0,0);
			lcdWritePos("1)Aquecer:",0,1);
			aquecedor.enableOutput();
			subEstado = 1;
			tempoUltMudancaTela = tempoRTC();
		break;
		case 1: //aquecendo
			
			if(tempoRTC() > tempoUltMudancaTela + 50){
				char textoTemp[5];
				temp2str(textoTemp, (float)ADC1->DR/(1<<12)*CTE_ADC_TEMP);
				lcdWritePos(textoTemp,11,1);
				lcdWrite("ºC");
				tempoUltMudancaTela = tempoRTC();
			}
			if( e[0] > -0.025 && e[0] < 0.025 &&
0
			){
				clearDisplay();
				lcdWritePos("Inic. Producao",0,0);
				lcdWritePos("2)Abast Reserv",0,1);
				subEstado = 2;
			}
		break;
		case 2: //encher reservatório
			if(!reservatorioCheio())
				abreValvula();
			else{
				fechaValvula();
				clearDisplay();
				lcdWritePos("Inic. Producao",0,0);
				lcdWritePos("3)Liga Esteira",0,1);
				subEstado = 3;
			}
		break;
		case 3:
			ligaEsteira();
			funNovoEstado = &estProducao;
		break;
	}
}
void estProducao(void){
	
}
void estEmergencia(void){
	int estadoPistao = statusPistao();
	int estadoEsteira = statusEsteira();
	int estadoValvula = statusValvula();
	int estadoAquecedor = aquecedor.getOutputEnable();
	fechaValvula();
	desativaPistao();
	desligaEsteira();
	aquecedor.disableOutput();
	clearDisplay();
	lcdWritePos("Parada de emergência",0,0);
	lcdWritePos("Sair [*]",0,1);
	while(le_teclado2() != '*'); //espera apertar botao para sair
	if(estadoAquecedor)
		aquecedor.enableOutput();
	if(estadoEsteira)
		ligaEsteira();
	if(estadoPistao)
		ativaPistao();
	if(estadoValvula)
		abreValvula();
}
void estLoginFechamento(void){
	
}
void estTrocaProducao(void){
	
}
void estFechamento(void){
	
}
void estRelatorio(void){
	
}
