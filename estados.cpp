#include "stm32f10x.h"
#include "estados.h"
#include "lcd.h"
#include "config_perifericos.h"
#include "TECLADOMOLE.h"
#include "PWM_T1CH2_A9.h"
#include "auxiliares.h"
#include "ComArduino.h"
#include "login_e_config.h"


extern int novoEstado;
extern int produtoConfigurado;
extern int produtoAtual;
extern int produtoNovo;
extern int tempoUltMudancaTela;
extern int subEstado;
extern char ultTecla;
extern PWM aquecedor;
extern float e[2];
extern float referencia;
extern float tempAlvo;

typedef void (*funcPointer)(void);
extern funcPointer funNovoEstado;
extern funcPointer funEstadoAnterior;

void estLoginInicial(void){
	int user = pedeUsuario();
	pedeSenha(user);
	produtoAtual = selecionaProduto();
	tempAlvo = ( produtoAtual == 1) ? 27.5 : 29.5; //se produto 1, tempAlvo é 27.5, se não, 29.5
	while(le_teclado2())
		atraso1m65s_lcd();
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
			if( e[0] > -0.25 && e[0] < 0.25 ){
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
	if(reservatorioVazio()) //controle do choco no reservatorio
		abreValvula();
	if (reservatorioCheio())
		fechaValvula();
	
	switch (subEstado){
		case 0: // inicia produção
			if(statusPistao())
				desativaPistao();
			
			ativaPistao();
			tempoAberto=tempoRTC();
			subEstado=1;
		break;
		case 2: //continua a executar o caso 1 , mas com o login de fechamento disponivel
			LoginFechamento();		
		case 1:
			if(statusPistao() && (tempoRTC() > tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
			}
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + 10)){
			ativaPistao();
			tempoAberto=tempoRTC();
			}
		break;			
	}
	if(ultTecla=='A')
		subEstado=2;
}
void estEmergencia(void){
	int estadoPistao = statusPistao();
	int estadoEsteira = statusEsteira();
	desativaPistao();
	desligaEsteira();
	clearDisplay();
	lcdWritePos("Parada de emergência",0,0);
	lcdWritePos("Sair [*]",0,1);
	if(ultTecla == '*')
		funNovoEstado = funEstadoAnterior;

}
void estLoginFechamento(void){
	
}
void estTrocaProducao(void){
	switch(subEstado){
	
		case 0: //espera esvaziar 
			if(statusPistao() && (tempoRTC() > tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
			}
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + 10)){
			ativaPistao();
			tempoAberto=tempoRTC();
			}
			if(reservatorioVazio())
				subEstado=1;
			break;			
		case 1:
			desligaEsteira();
			trocaProduto();//criar func 
			funNovoEstado = &estInicializacao;
			
		break;
	}
}
void estFechamento(void){
		if(!reservatorioVazio()){
		//espera esvaziar
	}else{
		desativaPistao();
		desligaEsteira();
		funNovoEstado = estR;
	}
}
void estRelatorio(void){
	
}
