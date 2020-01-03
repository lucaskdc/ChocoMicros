#include "stm32f10x.h"
#include "estados.h"
#include "lcd.h"
#include "config_perifericos.h"
#include "TECLADOMOLE.h"
#include "PWM_T1CH2_A9.h"
#include "auxiliares.h"
#include "ComArduino.h"
#include "login_e_config.h"
#include <string.h>


extern int estadoLogin;
extern char loginUser[7];
extern char loginPasswd[6];

extern int novoEstado;
extern int produtoAtual;
extern int produtoNovo;
extern int tempoUltMudancaTela;
extern int subEstado;
extern char ultTecla;
extern PWM aquecedor;
extern float e[2];
extern int unidadesProduzidas[2];

extern int tempoAberto, tempoFechado;

int tempoAberto, tempoFechado;

typedef void (*funcPointer)(void);
extern funcPointer funNovoEstado;
extern funcPointer funEstadoAnterior;

int loginFechamento(void);

void estLoginInicial(void){
	int user = pedeUsuario();
	pedeSenha(user);
	produtoNovo = selecionaProduto();
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
				if(produtoNovo == 1)
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
	if(ultTecla == '#'){
		funNovoEstado = &estInicializacao;
		produtoAtual = produtoNovo;
	}
	else if(ultTecla == 'D'){
		funNovoEstado = &estLoginInicial;
	}
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
	int retornoLogin;
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
			retornoLogin = loginFechamento();
		  switch(retornoLogin){
				case -1:
					subEstado = 1;
				break;
				case 1:
					//TROCAPRODUTO
				break;
				case 2:
					//ENCERRAPRODUCAO
				break;
			}
		case 1:
			if(statusPistao() && (tempoRTC() >= tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
			}
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + 10)){
				ativaPistao();
				tempoAberto=tempoRTC();
			}
		break;			
	}
	if(ultTecla=='A' && subEstado != 2){
		subEstado=2;
		estadoLogin = 0;
	}
}
int loginFechamento(void){
	int tamanho=0;
	int usuarioOK=0;

	const char operador[3][7] = {"288698","290431","288678"};
	int teste;
	switch(estadoLogin){
		case 0:
			loginUser[0] = 0;
			loginPasswd[0] = 0;
			clearDisplay();
			lcdWritePos("Digite o usuario:", 0, 0);
			lcdWritePos((const char*)loginUser, 1,4);
			estadoLogin = 1;
		
		case 1:
			tamanho = strlen((const char*)loginUser);
			if(ultTecla > 0x30 && ultTecla < 0x39 && tamanho<6){ //se a tecla eh um numero e o tamanho n chegou a 7=6
				setCursor(1,4+tamanho);
				loginUser[tamanho] = ultTecla;
				loginUser[++tamanho] = 0;				
				lcdWritechar(ultTecla);
			} else if(ultTecla == 'D'){
				if(tamanho == 0){
					estadoLogin = 0;
					return -1;
				}else{
					setCursor(1,4+tamanho-1);
					loginUser[tamanho-1] = 0;
					lcdWritechar(' ');
				}
			} else if(ultTecla == '#'){
				usuarioOK = 0;
				teste = 0;
				for(int j=0; j<2 && teste == 0; j++){
					int teste = 1;
					for(int i=0; i<6 && teste == 1; i++){
						if(loginUser[i] != operador[j][i])
							teste = 0;
					}
					if(teste){
						usuarioOK = 1;
					}
					if(usuarioOK){
						estadoLogin = 2;
						usuarioOK = j;
					} else {
						tempoUltMudancaTela = -9999;
						estadoLogin = -1;
					}
				}
			}
		break;
		case -1:
			clearDisplay();
			lcdWritePos("Usuário Inválido", 0, 0);
			tempoUltMudancaTela = tempoRTC();
			estadoLogin = 0;
			if(tempoUltMudancaTela + 100 < tempoRTC())
				estadoLogin = 0;
		break;
		case 2:
			tamanho = strlen((const char*)loginPasswd);
			if(ultTecla > 0x30 && ultTecla < 0x39 && tamanho<5){ //se a tecla eh um numero e o tamanho n chegou a 7=6
				setCursor(1,11+tamanho);
				loginPasswd[tamanho] = ultTecla;
				loginPasswd[++tamanho] = 0;				
				lcdWritechar('*');
			} else if(ultTecla == 'D'){
				if(tamanho == 0){
						tempoUltMudancaTela = -9999;
						estadoLogin = 0;
				}else{
					setCursor(1,11+tamanho-1);
					loginPasswd[tamanho-1] = 0;
					lcdWritechar(' ');
				}
			} else if(ultTecla == '#'){
				teste = 0;
				
				int teste = 1;
				for(int i=0; i<5 && teste == 1; i++){
					if(loginPasswd[i] != operador[usuarioOK][i])
						teste = 0;
				}
				if(teste){
					estadoLogin = 3;
					clearDisplay();
					lcdWritePos("1)Trocar Produto", 0,0);
					lcdWritePos("2)Encerrar Prod", 0,1);
				}				
			}
		break;
		case 3:
			if(ultTecla == 1){
				return 1;
			}
			if(ultTecla == 2){
				return 2;
			}
		break;
		
	}
	return 0; //EM PROCESSO
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
void LoginFechamento(void){
	
	
	
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
			produtoAtual = produtoNovo;
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
		funNovoEstado = estRelatorio;
	}
}
void estRelatorio(void){
	switch(subEstado){
		case 0:
			clearDisplay();
			lcdWritePos("RESUMO DE PROD",0,0);
			lcdWritePos("DE BOMBONS [#]",0,1);
			tempoUltMudancaTela = tempoRTC();
			if(ultTecla == '#'){
				subEstado = 1;
				tempoUltMudancaTela = -9999; //força atualização
			}
		break;
		case 1:
			clearDisplay();
			lcdWritePos("AO LEITE: ",0,0);
			lcdWriteInt(unidadesProduzidas[0], 2);
			lcdWritePos("MEIO AMARGO: ",0,1);
			lcdWriteInt(unidadesProduzidas[1], 2);
			if(ultTecla == '#'){
				funNovoEstado = estLoginInicial;
			}
		break;
	}
}
