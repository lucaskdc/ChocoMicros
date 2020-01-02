#include "stm32f10x.h"
#include "lcd.h"
//#include <tecladoduro.h>
#include "TECLADOMOLE.h"
#include "string.h"

////////////////////////////////////////////////////////////////////////////////
//para usar um teclado diferente, editar as três variáveis abaixo:            //
char teclaConfirma = '#';																											//
char teclaBackSpace = 'D';																										//
char semTeclaPressionada = '\0';																								//
////////////////////////////////////////////////////////////////////////////////

/*
//exemplo de teste na main, antes do for(;;)

		 int numUsuario = pedeUsuario();
		 pedeSenha(numUsuario);
		 int tipoDeChoco = selecionaProduto();
		 int velocidadeEsteira = selecionaVelocidade();

		 lcdWritePos("tipo:  ", 0, 0);
	   if(tipoDeChoco == 1) lcdWritePos("ao leite", 6, 0);
		 if(tipoDeChoco == 2) lcdWritePos("meio amargo", 6, 0);
		 lcdWritePos("|user:", 18 , 0); //apenas para lcd 24x2
		 
		 lcdWritePos("velocidade:  ", 0, 1);
		 if(velocidadeEsteira == 0) lcdWritePos("0", 12, 1);
		 if(velocidadeEsteira == 1) lcdWritePos("1", 12, 1);
		 if(velocidadeEsteira == 2) lcdWritePos("2", 12, 1);
		 if(velocidadeEsteira == 3) lcdWritePos("3", 12, 1);
		 if(numUsuario == 0) lcdWritePos("|lucas", 18, 1); //apenas para lcd 24x2
		 if(numUsuario == 1) lcdWritePos("|oskar", 18, 1); //apenas para lcd 24x2
		 if(numUsuario == 2) lcdWritePos("|jose ", 18, 1); //apenas para lcd 24x2
		 
*/

int pedeUsuario(){ //só sai após receber um usuário válido, retorna 0 para lucas, 1 para oskar, 2 para ze
	
		 char operador[3][7] = {"288698","290431","288678"}; //lucas, oskar e ze
		 char usuario[7] = {' ',' ',' ',' ',' ',' ','\0'};
		 
		 int pos = 0, sair = 0, numUser, notFirstTry = 0;
		 char leitura, leitura_ant;
	   char msg[2][25] = {"Digite o usuario:" , "invalido, repita:"};
		 
	   clearDisplay();
		 lcdWritePos(msg[notFirstTry], 0, 0);
		 
		 while(sair == 0){
		   leitura = le_teclado2();
			 //só entra um novo caractere após a ultima tecla for solta
			 //só funciona uma tecla de cada vez
		   if((leitura_ant == semTeclaPressionada) && (leitura != semTeclaPressionada)){
				 
				 if(leitura == teclaConfirma && pos == 6){ //caso confirmar e ja tem 6 digitos
					 int i;
					 for(i = 0; i < 3; i ++){
						 if(strcmp(usuario, operador[i]) == 0){
							 numUser = i;
							 sair = 1;
						 }
					 }
					 notFirstTry = 1; //se errar, vai mudar o print da linha de cima
					 pos = 0;      
					 for(i = 0; i < 6; i++){ //apaga a ultima tentativa
						 usuario[i] = ' ';
					 }
					 lcdWritePos(msg[notFirstTry], 0, 0);
					 lcdWritePos("      ", 0, 1);
					 lcdWritePos(usuario, 0, 1);
			   }
				 else{ //caso seja outra tecla
						if(leitura == teclaBackSpace && (pos >= 0)){ //se for backspace, apaga um digito
							if(pos > 0) pos--;
							usuario[pos] = ' ';
						}
						else{ //se for numero ou letra, acrescenta-o
							if(pos < 6){
								usuario[pos] = leitura;
								pos++;
							}
						}
						lcdWritePos(msg[notFirstTry], 0, 0);
						lcdWritePos("      ", 0, 1);
						lcdWritePos(usuario, 0, 1);
			  }
				 
				
			 }
		   leitura_ant = leitura;
		 }
		 clearDisplay();
		 return(numUser); //retorna 0 para lucas, 1 para oskar, 2 para ze
}


void pedeSenha(int numUser){ //senha é os primeiros 5 digitos do cartao
	
		 char senhaValida[3][6] = {"28869","29043","28867"}; //lucas, oskar e ze, senha de 5 digitos
		 char senha[6] = {' ',' ',' ',' ',' ','\0'};
		 
		 int  j, pos = 0, sair = 0, notFirstTry = 0;
		 char leitura, leitura_ant;
	   char msg[2][25] = {"Digite a senha:" , "invalido, repita:"};
		 
	   clearDisplay();
		 lcdWritePos(msg[notFirstTry], 0, 0);
		 
		 while(sair == 0){
		   leitura = le_teclado2();
			 //só entra um novo caractere após a ultima tecla for solta
			 //só funciona uma tecla de cada vez
		   if((leitura_ant == semTeclaPressionada) && (leitura != semTeclaPressionada)){
				 
				 if(leitura == teclaConfirma && pos == 5){ //caso confirmar e ja tem 5 digitos
					 int i;
					 if(strcmp(senha, senhaValida[numUser]) == 0){
							 sair = 1;
					 }
					 notFirstTry = 1; //se errar, vai mudar o print da linha de cima
					 pos = 0;      
					 for(i = 0; i < 5; i++){ //apaga a ultima tentativa
						 senha[i] = ' ';
					 }
					 lcdWritePos(msg[notFirstTry], 0, 0);
					 lcdWritePos("      ", 0, 1);
					 setCursor(0,1);
					 for(j = 0; j < pos; j++){
						 lcdWritechar('*');
					 }
			   }
				 else{ //caso seja outra tecla
						if(leitura == teclaBackSpace && (pos >= 0)){ //se for backspace, apaga um digito
							if(pos > 0) pos--;
							senha[pos] = ' ';
						}
						else{ //se for numero ou letra, acrescenta-o
							if(pos < 5){
								senha[pos] = leitura;
								pos++;
							}
						}
						lcdWritePos(msg[notFirstTry], 0, 0);
						lcdWritePos("      ", 0, 1);
					  setCursor(0,1);
					  for(j = 0; j < pos; j++){
						  lcdWritechar('*');
					  }
			  }
				 
				
			 }
		   leitura_ant = leitura;
		 }
		 clearDisplay();
}

int selecionaProduto(){ //retorna 1 para ao leite, 2 para meio amargo
	
	int produtoSelecionado = 0, sair = 0;
	char leitura, leitura_ant;
	char descricao[2][25] = {"[1] ao leite" , "[2] meio amargo"};
	clearDisplay();
	lcdWritePos("sel. o produto", 0, 0);
	lcdWritePos("pressione 1 ou 2", 0, 1);
	
	while(sair == 0){
		leitura = le_teclado2();
		if((leitura_ant == semTeclaPressionada) && (leitura != semTeclaPressionada)){
			if(produtoSelecionado != 0 && leitura == teclaConfirma){
				sair = 1;
			}
			else{
				switch(leitura){
					case '1':
						clearDisplay();
						lcdWritePos(descricao[0], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						produtoSelecionado = 1;
					break;
					case '2':
						clearDisplay();
						lcdWritePos(descricao[1], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						produtoSelecionado = 2;
					break;
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '0':
					case '*':
					case 'X':
					case 'U':
					case 'D':
					case 'L':
					case 'R':
					case 'A':
					case 'B':
					case 'C':
					case '#':
					case 'E':
						clearDisplay();
						lcdWritePos("sel. o produto", 0, 0);
						lcdWritePos("pressione 1 ou 2", 0, 1);
						produtoSelecionado = 0;
					break;
				}
			}
	  }
		leitura_ant = leitura;
	}
	return(produtoSelecionado);
	
}

int selecionaVelocidade(){ //retorna um int de 0 a 3 que é a velocidade da esteira

	int velocidadeSelecionada = -1, sair = 0;
	char leitura, leitura_ant;
	char descricao[4][25] = {"[0] minima" , "[1] baixa", "[2] alta ", "[3] maxima" };
	clearDisplay();
	lcdWritePos("escolha a vel.", 0, 0);
	lcdWritePos("pressione 0 a 3", 0, 1);
	
	while(sair == 0){
		leitura = le_teclado2();
		if((leitura_ant == semTeclaPressionada) && (leitura != semTeclaPressionada)){
			if(velocidadeSelecionada != -1 && leitura == teclaConfirma){
				sair = 1;
			}
			else{
				switch(leitura){
					case '0':
						clearDisplay();
						lcdWritePos(descricao[0], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						velocidadeSelecionada = 0;
					break;
					case '1':
						clearDisplay();
						lcdWritePos(descricao[1], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						velocidadeSelecionada = 1;
					break;
					case '2':
						clearDisplay();
						lcdWritePos(descricao[2], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						velocidadeSelecionada = 2;
					break;
					case '3':
						clearDisplay();
						lcdWritePos(descricao[3], 0, 0);
						lcdWritePos("favor confirmar", 0, 1);
						velocidadeSelecionada = 3;
					break;
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					case '*':
					case 'X':
					case 'U':
					case 'D':
					case 'L':
					case 'R':
					case 'A':
					case 'B':
					case 'C':
					case '#':
					case 'E':
						clearDisplay();
						lcdWritePos("escolha a vel.", 0, 0);
						lcdWritePos("pressione 0 a 3", 0, 1);
						velocidadeSelecionada = -1;
					break;
				}
			}
	  }
		leitura_ant = leitura;
	}
	clearDisplay();
	return(velocidadeSelecionada);
}
