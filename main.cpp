#include "stm32f10x.h"
#include "PWM_T1CH2_A9.h"
#include "lcd.h"
#include "TECLADOMOLE.h"
#include <string.h>
#include "config_perifericos.h"
#include "auxiliares.h"
#include "login_e_config.h"
#include "estados.h"


PWM aquecedor;
const float Kp=2, Kd=0.5;
volatile float e[2] = {0};
volatile float referencia = 0.50; //50%
volatile float tempAlvo = 27.5;
volatile int produtoAtual = 1;

volatile char ultTecla = 0;
volatile int produtoConfigurado = 1;
volatile int tempoUltMudancaTela = 0;

volatile int subEstadoAnterior = 0;
volatile int subEstado = 0;

typedef void (*funcPointer)(void);
funcPointer funEstadoAnterior = &estConfirma;
funcPointer funEstado = &estConfirma;
funcPointer funNovoEstado = &estConfirma;


void trocaEstado(funcPointer p){
	funEstadoAnterior = funEstado;
	subEstadoAnterior = subEstado;
	subEstado = 0;
	funEstado = p;
}

extern "C" void ADC1_2_IRQHandler(){
	e[1] = e[0];
	e[0] = (float)ADC1->DR/(1<<12) - referencia; //normalizado 0 a 1
	float u = Kp*e[0] + Kd*(e[0]-e[1]);
	aquecedor.setDutyCycle(u*100);
}

int main(){
	configuraClock();
	configRTC();
	configADC();
	
	aquecedor = PWM();	//inicializa PWM, aquecedor � var global
	
	lcdConfig();
	lcdWrite("OLA");

	configura_portaB_teclado2();
//	int i=0;
	//char vect[16];
	
	int user = pedeUsuario();
	pedeSenha(user);
	produtoAtual = selecionaProduto();
	tempAlvo = ( produtoAtual == 1) ? 27.5 : 29.5; //se produto 1, tempAlvo � 27.5, se n�o, 29.5
	
	for(;;){
		if(funNovoEstado != funEstado)
			trocaEstado(funNovoEstado);
		funEstado();
		ultTecla = le_teclado2();
		if(ultTecla == '*')
			estEmergencia();
	}
}

