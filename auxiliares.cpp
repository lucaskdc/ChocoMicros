////////////////////
//Funções auxiliares, fora do main.cpp
////////////////////
#include "auxiliares.h"
void porcentagemFloat2Str(char text[6], float val){
	text[0]=(int)val%100/10+0x30;
	text[1]=(int)val%10+0x30;
	text[2]='.';
	text[3]=(int)(val*10)%10+0x30;
	text[4]=(int)(val*100)%10+0x30;
	text[5]=0;
}
void temp2str(char text[5], float val){
	if((int)(val*100)%10 >=5)//val = ab.cdef *100-> abcd.ef %10 -> d se d>5 ->arredonda para cima
		val += 0.1;
	text[0]=(int)val%100/10+0x30;
	text[1]=(int)val%10+0x30;
	text[2]='.';
	text[3]=(int)(val*10)%10+0x30;
	text[4]=0;
}
