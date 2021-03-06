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
#include "CONSTANTES.h"

extern int estadoLogin;
extern char loginUser[7];
extern char loginPasswd[6];
//extern int usuarioLogando;
volatile int usuarioLogando=-1;


extern int novoEstado;
extern int produtoAtual;
//extern int produtoNovo;
extern int tempoUltMudancaTela;
extern int subEstado;
extern char ultTecla;
extern PWM aquecedor;
extern float e[2];
extern int unidadesProduzidas[2];
extern int tempoAberto, tempoFechado;

typedef void (*funcPointer)(void);
extern funcPointer funNovoEstado;
extern funcPointer funEstadoAnterior;

int loginFechamento(void);

void estLoginInicial(void){
	int user;
	unidadesProduzidas[0] = 0;
	unidadesProduzidas[1] = 0;

	switch(subEstado){
		case 0:
			if(ultTecla == 0)
				subEstado = 1;
		break;
		case 1:
			user = pedeUsuario(); //trava programa
			pedeSenha(user); //trava programa
			produtoAtual = selecionaProduto(); //trava programa
			subEstado = 2;
		break;
		case 2:
			if(ultTecla == 0)
				funNovoEstado = estConfirma;
		break;
	}
}
void estConfirma(void){
	switch (subEstado){
		case 2: if(tempoRTC() > tempoUltMudancaTela + 100)
		case 0:
			{
				clearDisplay();
				lcdWritePos("Confirmar Produ��o?",0,0);
				if(produtoAtual == 1)
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
		//produtoAtual = produtoNovo;
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
				lcdWriteTemp(11,1);
				tempoUltMudancaTela = tempoRTC();
			}
			if( e[0] >= -0.5 && e[0] <= 0.5 ){
				clearDisplay();
				lcdWritePos("Inic. Producao",0,0);
				lcdWritePos("2)Abast Reserv",0,1);
				subEstado = 2;
			}
		break;
		case 2: //encher reservat�rio
			if(reservatorioVazio())
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

void refreshQTDeTempLCD(void){
	setCursor(4,1);
	lcdWriteInt(unidadesProduzidas[produtoAtual-1], 5);
	lcdWriteTemp(10,1);
	tempoUltMudancaTela = tempoRTC();
}
int produzidosUltimoAbastecimento = 0;
void estProducao(void){
	if(subEstado == 0)
		produzidosUltimoAbastecimento = 0;
	
	int retornoLogin;
	if(!reservatorioCheio() && produzidosUltimoAbastecimento > 35){
		abreValvula();
		produzidosUltimoAbastecimento = 0;
	}
		
	if(reservatorioVazio()) //controle do choco no reservatorio
		abreValvula();
	if (reservatorioCheio())
		fechaValvula();

	switch (subEstado){
		case 0: // inicia produ��o
			if(statusPistao())
				desativaPistao();
			clearDisplay();
			if(produtoAtual == 1){
				lcdWritePos("Prod. Ao Leite", 0, 0);
				lcdWritePos("QTD:", 0,1);
				lcdWriteInt(unidadesProduzidas[0], 5);
			}else{
				lcdWritePos("Prod. Meio Amarg", 0, 0);
				lcdWritePos("QTD:", 0,1);
				lcdWriteInt(unidadesProduzidas[1], 5);
			}
			lcdWriteTemp(10,1);
			ativaPistao();
			tempoAberto=tempoRTC();
			subEstado=1;
		break;
		case 2: //continua a executar o caso 1 , mas com o login de fechamento disponivel
			retornoLogin = loginFechamento();
		  switch(retornoLogin){
				case -1: //limpa tela e volta ao subestado de exibir valores de produ��o
					clearDisplay();
					if(produtoAtual == 1){
						lcdWritePos("Prod. Ao Leite", 0, 0);
						lcdWritePos("QTD:", 0,1);
						lcdWriteInt(unidadesProduzidas[0], 5);
					}else{
						lcdWritePos("Prod. Meio Amarg", 0, 0);
						lcdWritePos("QTD:", 0,1);
						lcdWriteInt(unidadesProduzidas[1], 5);
					}
					lcdWriteTemp(10,1);
					subEstado = 1;
				break;
				case 1:
					funNovoEstado = &estTrocaProducao;
				break;
				case 2:
					funNovoEstado = &estFechamento;
				break;
			}
		//sem break
		case 1:
			if(statusPistao() && (tempoRTC() >= tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
				unidadesProduzidas[produtoAtual-1]++; //incrementa
				produzidosUltimoAbastecimento++;
				
				//if(subEstado == 1)
					//refreshQTDeTempLCD(); //s� atualiza se n�o estiver no estado de Login
			}
			if(subEstado == 1 && tempoRTC() > tempoUltMudancaTela + 10)
					refreshQTDeTempLCD(); //s� atualiza se n�o estiver no estado de Login
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + TEMPOPISTAOFECHADO)){
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
	int i,j;
	
	const char operador[3][7] = {"288698","290431","288678"};
	int teste;
	switch(estadoLogin){
		case 11:
			if(ultTecla == 0)
				estadoLogin = 1;
		break;
		case 0:
			loginUser[0] = 0;
			loginPasswd[0] = 0;
			clearDisplay();
			lcdWritePos("Digite o usuario:", 0, 0);
			lcdWritePos((const char*)loginUser, 1,4);
			estadoLogin = 1;
		
		case 1:
			tamanho = strlen((const char*)loginUser);
			if(ultTecla >= 0x30 && ultTecla <= 0x39 && tamanho<6){ //se a tecla eh um numero e o tamanho n chegou a 7=6
				setCursor(tamanho,1);
				loginUser[tamanho] = ultTecla;
				loginUser[++tamanho] = 0;				
				lcdWritechar(ultTecla);
			} else if(ultTecla == 'D'){
				if(tamanho == 0){
					estadoLogin = 0;
					return -1;
				}else{
					setCursor(tamanho-1,1);
					loginUser[tamanho-1] = 0;
					lcdWritechar(' ');
				}
			} else if(ultTecla == '#'){
				usuarioOK = 0;
				teste = 0;
				
				
				for(j=0; j<3 && teste == 0; j++){
					teste = 1;
					for(i=0; i<6 && teste == 1; i++){
						if(loginUser[i] != operador[j][i])
							teste = 0;
					}
					if(teste){
						usuarioOK = 1;
					}
					if(usuarioOK){
						estadoLogin = 20;
						usuarioLogando = j;
					} else {
						tempoUltMudancaTela = -9999;
						estadoLogin = -11;
					}
				}
			}
			if(estadoLogin == 1 && ultTecla != 0)
				estadoLogin = 11;
		break;
		case -11:
			clearDisplay();
			lcdWritePos("Usu�rio Inv�lido", 0, 0);
			tempoUltMudancaTela = tempoRTC();
			estadoLogin = -1;
		break;
		case -1:
			if(tempoUltMudancaTela + 100 < tempoRTC())
				estadoLogin = 0;
		break;
		case 22:
			if(ultTecla == 0)
				estadoLogin = 2;
		break;
		case 20:
			if(ultTecla == 0)
				estadoLogin = 21;
		break;
		case 21:
			lcdWritePos("Digite a Senha: ", 0, 0);
			setCursor(11,1);
			for(int i=0; i<strlen(loginPasswd); i++){
				lcdWritechar('*');
			}
			//lcdWritePos((const char*)loginUser, 4,1);
			estadoLogin = 2;
		break;
		case 2:
			tamanho = strlen((const char*)loginPasswd);
			if(ultTecla >= 0x30 && ultTecla <= 0x39 && tamanho<5){ //se a tecla eh um numero e o tamanho n chegou a 7=6
				setCursor(10+tamanho,1);
				loginPasswd[tamanho] = ultTecla;
				loginPasswd[++tamanho] = 0;				
				lcdWritechar('*');
			} else if(ultTecla == 'D'){
				if(tamanho == 0){
						tempoUltMudancaTela = -9999;
						estadoLogin = 0;
				}else{
					setCursor(10+tamanho-1,1);
					loginPasswd[tamanho-1] = 0;
					lcdWritechar(' ');
				}
			} else if(ultTecla == '#'){
				teste = 1;
				for(int i=0; i<5 && teste == 1; i++){
					if(loginPasswd[i] != operador[usuarioLogando][i])
						teste = 0;
				}
				if(teste){
					estadoLogin = 3;
					clearDisplay();
					lcdWritePos("1)Trocar Produto", 0,0);
					lcdWritePos("2)Encerrar Prod", 0,1);
				}

			}
			if(estadoLogin == 2 && ultTecla != 0){
				estadoLogin = 22;
			}
		break;
		case 3:
			if(ultTecla == '1'){
				return 1;
			}
			if(ultTecla == '2'){
				return 2;
			}
		break;
		
	}
	return 0; //EM PROCESSO
}

void estEmergencia(void){
	switch(subEstado){
		case 0:
			  fechaValvula();
				desativaPistao();
				desligaEsteira();
				clearDisplay();
				lcdWritePos("Parada de emerg�ncia",0,0);
				lcdWritePos("Sair [*]",0,1);
				subEstado = 1;
		break;
	//int estadoPistao = statusPistao();
	//int estadoEsteira = statusEsteira();
		case 1:
			if(ultTecla == 0)
				subEstado = 2;
		break;
		case 2:
			if(ultTecla == '*')
				if(funEstadoAnterior == estProducao || funEstadoAnterior == estTrocaProducao || funEstadoAnterior == estFechamento){
					ligaEsteira();
					funNovoEstado = funEstadoAnterior;
				}
	}
}
void estTrocaProducao(void){
	switch(subEstado){
		case 0:
			clearDisplay();
			lcdWritePos("Trocando Producao", 0, 0);
			lcdWritePos("ESVAZIANDO RESERV", 0, 1);
			subEstado = 1;
			fechaValvula();
		case 1: //espera esvaziar 
			if(statusPistao() && (tempoRTC() > tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
				unidadesProduzidas[produtoAtual-1]++; //incrementa
			}
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + TEMPOPISTAOFECHADO)){
			ativaPistao();
			tempoAberto=tempoRTC();
			}
			if(reservatorioVazio())
				subEstado=2;
		break;			
		case 2:
			desligaEsteira();
			if(produtoAtual == 1)
				produtoAtual = 2;
			else
				produtoAtual = 1;
			funNovoEstado = &estInicializacao;
			
		break;
	}
}
void estFechamento(void){
	switch(subEstado){
		case 0:
			clearDisplay();
			lcdWritePos("Fechando Producao", 0, 0);
			lcdWritePos("ESVAZIANDO RESERV", 0, 1);
			subEstado = 1;
			fechaValvula();
		case 1: //espera esvaziar 
			if(statusPistao() && (tempoRTC() > tempoAberto + 100)){
				desativaPistao();
				tempoFechado=tempoRTC();
				unidadesProduzidas[produtoAtual-1]++; //incrementa

			}
			if(!(statusPistao()) && (tempoRTC() > tempoFechado + TEMPOPISTAOFECHADO)){
			ativaPistao();
			tempoAberto=tempoRTC();
			}
			if(reservatorioVazio())
				subEstado=2;
		 break;
			
		case 2:
			desativaPistao();
			desligaEsteira();
			funNovoEstado = &estRelatorio;
		break;
	}
}
void estRelatorio(void){
	switch(subEstado){
		case 0:
			clearDisplay();
			lcdWritePos("RESUMO DE PROD",0,0);
			lcdWritePos("DE BOMBONS [#]",0,1);
			tempoUltMudancaTela = tempoRTC();
			if(ultTecla == 0)
				subEstado = 1;
		break;
		case 1:
			if(ultTecla == '#'){
				subEstado = 2;
			}
		break;
		case 2:
			clearDisplay();
			lcdWritePos("AO LEITE:  ",0,0);
			lcdWriteInt(unidadesProduzidas[0], 5);
			lcdWritePos("1/2 AMARGO:",0,1);
			lcdWriteInt(unidadesProduzidas[1], 5);
			if(ultTecla == 0)
				subEstado = 3;
		break;
		case 3:
			if(ultTecla == '#'){
				funNovoEstado = estLoginInicial;
			}
		break;
	}
}
