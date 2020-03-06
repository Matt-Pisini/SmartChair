#ifndef LCD_CONTROL

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(unsigned char);
void lcd_writedata(unsigned char);
void lcd_writebyte(unsigned char);
void lcd_writenibble(unsigned char);
void lcd_wait(void);
void lcd_stringout_P(char *);
void lcd_row_blink(unsigned char);
void lcd_clear();
void lcd_string_state_P(const char * const s[], int size, int index);
void lcd_cursor(int row);

/****************************************************************************************************/

#define LCD_CONTROL
#endif