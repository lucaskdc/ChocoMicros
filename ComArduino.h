///////////////////////////
//Biblioteca para comunicação com interface para Computador (Arduino)
////////////////////////////
#ifndef COM_ARDUINO_H
#define COM_ARDUINO_H
	inline int reservatorioCheio();
	inline int reservatorioVazio();
	inline void ativaPistao(void);
	inline void desativaPistao(void);
	inline void ligaEsteira(void);
	inline void desligaEsteira(void);
	inline void abreValvula(void);
	inline void fechaValvula(void);
#endif
