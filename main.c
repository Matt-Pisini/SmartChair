/***************************************************************

EE459 Final Project Main Source Code

Team Members: 	Matt Pisini, Jessica Mow, Zechen Zhou
Date: 			2/25/20
Purpose:		Code controlling all SmartChair functionality

**************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h> 
#include "PWM.h"
#include "LCD.h"

volatile uint8_t flag;

int int main( void )
{
    lcd_init();                 // Initialize the LCD display
    lcd_writecommand(0x01);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);
    while (1) {   }              // Loop forever

	return 0;
}

ISR (TIMER1_COMPA_vect)  // timer0 overflow interrupt service routine
{
  flag =  1;
  lcd_stringout("interrupt");
    // if (flag == 1)    //event to be exicuted every 4ms here
    // {
    //   flag = 0;
    // }     
    // else
    // {
    //   flag = 1;
    // }
}