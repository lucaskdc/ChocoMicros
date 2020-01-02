///////////////////////////
//Biblioteca para comunicação com interface para Computador (Arduino)
////////////////////////////
#ifndef COM_ARDUINO_H
#define COM_ARDUINO_H
	inline int reservatorioCheio(void);
	inline int reservatorioVazio(void);
	
	inline int statusPistao(void); //1 ativo, 0 desativado
	inline int statusEsteira(void); //1 ligada, 0 desligada
	inline int statusValvula(void); //1 aberta, 0 fechada
	
	inline void ativaPistao(void);
	inline void desativaPistao(void);
	inline void ligaEsteira(void);
	inline void desligaEsteira(void);
	inline void abreValvula(void);
	inline void fechaValvula(void);
#endif
