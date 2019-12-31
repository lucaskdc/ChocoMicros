
#ifndef TECLADODURO_H_
#define TECLADODURO_H_

void configura_portaB(void);
void atraso_debounce(void);
int debounce(int linha, int coluna);
char ij_to_char(int linha, int coluna);
char le_teclado(); //retorna a tecla pressionada.

#endif

