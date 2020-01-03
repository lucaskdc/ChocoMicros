////////////////////////
//Faz a configura��o de alguns perif�ricos
////////////////////////
#ifndef CONFIG_PERIFERICOS_H
#define CONFIG_PERIFERICOS_H
void configuraClock(void);
void configGPIOs(void);
void configRTC(void);
int tempoRTC(void);
void configADC(void);
void configA8MCO(void); //configura pino A8 para sa�da MCO (para fins de teste de clock)
#endif
