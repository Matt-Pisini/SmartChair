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

int main( void )
{
    lcd_init();                 // Initialize the LCD display
    lcd_writecommand(0x01);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);
    timer_init();
    init_servo();
    lcd_stringout("Initialized");

    while (1)  // Loop forever
    {   
    	
    }             

	return 0;
}

