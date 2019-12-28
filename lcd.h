
#ifndef LCD_H_
#define LCD_H_

void atraso_40us_lcd(void);
void atraso_1m65s_lcd(void);
void lcd_comando(char data_display);
void lcdWritechar(char data_display);
void clear_display();
void lcd_config();
void setCursor(unsigned int coluna, unsigned int linha);
void lcdWrite(char string1[16]);

#endif
