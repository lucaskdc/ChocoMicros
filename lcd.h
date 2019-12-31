#ifndef LCD_H_
#define LCD_H_
inline void pulseEnable(void);
void atraso40us_lcd(void);
void atraso1m65s_lcd(void);
void lcdComando(char data_display);
void lcdWritechar(char data_display);
void clearDisplay();
void lcdConfig();
void setCursor(unsigned int coluna, unsigned int linha);
void lcdWrite(char string1[16]);
void lcdWritePos(char string1[16], unsigned int coluna, unsigned int linha);
#endif
