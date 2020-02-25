#ifndef LCD_CONTROL
#define LCD_CONTROL

#include <avr/pgmspace.h>
#include <avr/io.h>

#define NIBBLE_HIGH                 // Use bits 4-7 for talking to LCD

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(unsigned char);
void lcd_writedata(unsigned char);
void lcd_writebyte(unsigned char);
void lcd_writenibble(unsigned char);
void lcd_wait(void);
void lcd_stringout_P(char *);

#ifdef NIBBLE_HIGH
const unsigned char str1[] PROGMEM = ">> at328-5.c hi <<901234";
#else
const unsigned char str1[] PROGMEM = ">> at328-5.c lo <<901234";
#endif
const unsigned char str2[] PROGMEM = ">> USC EE459L <<78901234";

#define LCD_RS          (1 << PB1)
#define LCD_RW          (1 << PB3)
#define LCD_E           (1 << PB0)
#define LCD_Bits        (LCD_RS|LCD_RW|LCD_E)

#ifdef NIBBLE_HIGH
#define LCD_Data_D     0xf0     // Bits in Port D for LCD data
#define LCD_Status     0x80     // Bit in Port D for LCD busy status
#else
#define LCD_Data_B     0x03     // Bits in Port B for LCD data
#define LCD_Data_D     0x0c     // Bits in Port D for LCD data
#define LCD_Status     (1 << PD7) // Bit in Port D for LCD busy status
#endif

#endif