////////////////////////
//Faz a configuração de alguns periféricos
////////////////////////
#ifndef CONFIG_PERIFERICOS_H
#define CONFIG_PERIFERICOS_H
void configuraClock(void);
void configGPIOs(void);
void configRTC(void);
int tempoRTC(void);
void configADC(void);
void configA8MCO(void); //configura pino A8 para saída MCO (para fins de teste de clock)
#endif
