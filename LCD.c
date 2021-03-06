/**********************************************************************

Source: "at328-5.c - Demonstrate interface to a parallel LCD display"

***********************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "LCD.h"

#define NIBBLE_HIGH                 // Use bits 4-7 for talking to LCD

#define LCD_RS          (1 << PB0)
#define LCD_RW          0//(1 << PB3)
#define LCD_E           (1 << PB1)
#define LCD_Bits        (LCD_RS|LCD_E)

#ifdef NIBBLE_HIGH
#define LCD_Data_D     0xf0     // Bits in Port D for LCD data
#define LCD_Status     0x80     // Bit in Port D for LCD busy status
#else
#define LCD_Data_B     0x03     // Bits in Port B for LCD data
#define LCD_Data_D     0x0c     // Bits in Port D for LCD data
#define LCD_Status     (1 << PD7) // Bit in Port D for LCD busy status
#endif

const uint8_t row_offsets[4] = {0x00, 0x40, 0x14, 0x54};    //values for starting points of LCD

/*
  lcd_stringout_P - Print the contents of the character string "s" starting at LCD
  RAM location "x" where the string is stored in ROM.  The string must be
  terminated by a zero byte.
*/
void lcd_stringout_P(char *s)
{
    unsigned char ch;

    /* Use the "pgm_read_byte()" routine to read the date from ROM */
    while ((ch = pgm_read_byte(s++)) != '\0') {
        lcd_writedata(ch);             // Output the next character
    }
}

/*
  lcd_string_state_P - It prints each element of the state on successive rows.

  Takes in 3 arguments:
  1)A pointer to state_x, which is an array of pointers to cstrings(char *)
    that correspond to each row of the state.
  2)Size of the arrrray of pointers, i.e. how many elements there are.
  3)Index, which is the first elements of the array to print on LCD row 0.

*/

void lcd_string_state_P(const char * const s[], int size, int index)
{
    char buffer[20];
    lcd_moveto(0, 1);
    int iterator;
    int top_value;
    int lcd_row = 1;

    //Display state has 4 or less rows of text
    if (size <= 4)
    {
      iterator = 0;
      top_value = size;
    }
    //Display screen has more than 4 rows of text
    else
    {
      iterator = index;
      top_value = iterator + 4;
    }

    //Display next 4 lines of text on the LCD
    for (; iterator < top_value; iterator++)
    {
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(s[iterator])));
        lcd_stringout(buffer);
        lcd_moveto(lcd_row, 1);
        lcd_row++;
    }

}

/*
  lcd_cursor - displays the cursor value on particular row of LCD.
*/
void lcd_cursor(int row)
{
  lcd_moveto(row, 0);
  lcd_writedata('>');
  lcd_moveto(row, 0);
}

/*
  lcd_init - Do various things to force a initialization of the LCD
  display by instructions, and then set up the display parameters and
  turn the display on.
*/
void lcd_init(void)
{
#ifdef NIBBLE_HIGH
    DDRD |= LCD_Data_D;         // Set PORTD bits 4-7 for output
#else
    DDRB |= LCD_Data_B;         // Set PORTB bits 0-1 for output
    DDRD |= LCD_Data_D;         // Set PORTD bits 2-3 for output
#endif
    DDRB |= LCD_Bits;           // Set PORTB bits 2, 3 and 4 for output

    PORTB &= ~LCD_RS;           // Clear RS for command write

    _delay_ms(15);              // Delay at least 15ms

    lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
    _delay_ms(5);               // Delay at least 4msec

    lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
    _delay_us(120);             // Delay at least 100usec

    lcd_writenibble(0x30);      // Send 00110000, set for 8-bit interface
    _delay_ms(2);
    
    lcd_writenibble(0x20);      // Send 00100000, set for 4-bit interface
    _delay_ms(2);

    lcd_writecommand(0x28);     // Function Set: 4-bit interface, 2 lines

    lcd_writecommand(0x0f);     // Display and cursor on
}

/*
  lcd_ to - Move the cursor to the row and column given by the arguments.
  Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
    unsigned char pos;
    pos = row_offsets[row] + col;
    lcd_writecommand(0x80 | pos);
}

void lcd_clear()
{
  lcd_writecommand(1);
}

/*
  lcd_stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void lcd_stringout(char *str)
{
    char ch;
    while ((ch = *str++) != '\0')
    lcd_writedata(ch);
}

/*
  lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char x)
{
    PORTB &= ~LCD_RS;       // Clear RS for command write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char x)
{
    PORTB |= LCD_RS;  // Set RS for data write
    lcd_writebyte(x);
    lcd_wait();
}

/*
  lcd_writebyte - Output a byte to the LCD
*/
void lcd_writebyte(unsigned char x)
{
    lcd_writenibble(x);
    lcd_writenibble(x << 4);
}

/*
  lcd_writenibble - Output a 4-bit nibble to the LCD
*/
void lcd_writenibble(unsigned char x)
{
#ifdef NIBBLE_HIGH
    PORTD &= ~LCD_Data_D;
    PORTD |= (x & LCD_Data_D);  // Put high 4 bits of data in PORTD
#else
    PORTB &= ~LCD_Data_B;
    PORTB |= (x & LCD_Data_B);  // Put low 2 bits of data in PORTB
    PORTD &= ~LCD_Data_D;
    PORTD |= (x & LCD_Data_D);  // Put high 2 bits of data in PORTD
#endif
    PORTB &= ~(LCD_RW|LCD_E);   // Set R/W=0, E=0
    PORTB |= LCD_E;             // Set E to 1
    PORTB |= LCD_E;             // Extend E pulse > 230ns
    PORTB &= ~LCD_E;            // Set E to 0
}

/*
  lcd_wait - Wait for the BUSY flag to reset
*/
void lcd_wait()
{
#ifdef USE_BUSY_FLAG
    unsigned char bf;

#ifdef NIBBLE_HIGH
    PORTD &= ~LCD_Data_D;       // Set for no pull ups
    DDRD &= ~LCD_Data_D;        // Set for input
#else
    PORTB &= ~LCD_Data_B;       // Set for no pull ups
    PORTD &= ~LCD_Data_D;
    DDRB &= ~LCD_Data_B;        // Set for input
    DDRD &= ~LCD_Data_D;
#endif

    PORTB &= ~(LCD_E|LCD_RS);   // Set E=0, R/W=1, RS=0
    PORTB |= LCD_RW;

    do {
        PORTB |= LCD_E;         // Set E=1
        _delay_us(1);           // Wait for signal to appear
        bf = PIND & LCD_Status; // Read status register high bits
        PORTB &= ~LCD_E;        // Set E=0
  PORTB |= LCD_E;         // Need to clock E a second time to fake
  PORTB &= ~LCD_E;        //   getting the status register low bits
    } while (bf != 0);          // If Busy (PORTD, bit 7 = 1), loop

#ifdef NIBBLE_HIGH
    DDRD |= LCD_Data_D;         // Set PORTD bits for output
#else
    DDRB |= LCD_Data_B;         // Set PORTB, PORTD bits for output
    DDRD |= LCD_Data_D;
#endif
#else
    _delay_ms(2);   // Delay for 2ms
#endif
}