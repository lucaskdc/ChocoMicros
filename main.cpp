#include "stm32f10x.h"
#include "PWM_T1CH2_A9.h"
#include "lcd.h"
#include "TECLADOMOLE.h"
#include <string.h>
#include "config_perifericos.h"
#include "auxiliares.h"
#include "login_e_config.h"
#include "estados.h"
#include "CONSTANTES.h"

PWM aquecedor;
const float Kp=1.3, Kd=0.5;
volatile float e[2] = {0};
volatile int unidadesProduzidas[2]={0};
volatile int produtoAtual = 1;
volatile int produtoNovo = 1;

volatile char ultTecla = 0;
volatile int tempoUltMudancaTela = 0;

volatile int subEstadoAnterior = 0;
volatile int subEstado = 0;
volatile int estadoLogin = 0;
volatile char loginUser[7];
volatile char loginPasswd[6];

volatile int tempoAberto=0, tempoFechado=0;

typedef void (*funcPointer)(void);
funcPointer funEstadoAnterior = &estConfirma;
funcPointer funEstado = &estConfirma;
funcPointer funNovoEstado = &estConfirma;


void trocaEstado(funcPointer p){
	funEstadoAnterior = funEstado;
	subEstadoAnterior = subEstado;
	subEstado = 0;
	funEstado = p;
	tempoUltMudancaTela = -9999;
}

extern "C" void ADC1_2_IRQHandler(){
	float tempAlvo = ( produtoAtual == 1) ? 27.5 : 29.5;
	e[1] = e[0];
	e[0] = tempAlvo - CTE_ADC_TEMP*(float)ADC1->DR/(1<<12); //normalizado 0 a 1
	float u = Kp*e[0] + Kd*(e[0]-e[1]);
	aquecedor.setDutyCycle(u*100);
}

int main(){
	configuraClock();
	configGPIOs();
	configRTC();
	configADC();
	aquecedor = PWM();	//inicializa PWM, aquecedor é var global
	lcdConfig();
	
	//int user = pedeUsuario();
	//pedeSenha(user);
	//produtoNovo = selecionaProduto();
	funNovoEstado = estProducao;
	while(le_teclado2())
		atraso1m65s_lcd();
	for(;;){
		if(funNovoEstado != funEstado)
			trocaEstado(funNovoEstado);
		funEstado();
		ultTecla = le_teclado2();
		if(ultTecla == '*')
			estEmergencia();
	}
}

