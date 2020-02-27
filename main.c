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
#include "serial.h"
#include "ADC.h"

/****************************DEFINITIONS***************************/

#define BAUD_RATE 47
#define PWM_COUNTER OCR1A       //increase/decrease to move servo
#define ADC_VALUE ADCH          //register where 8-bit ADC value kept



int main( void )
{
    sei();                      //enable global interrupts

    /**************** INITIALIZATIONS ***************/

    lcd_init();                 //Initialize the LCD display
    timer_init();               //Initialize the PWM timer 
    init_servo();               //Initialize servo 

    ADC_FLAG = 0;               //Initializes ADC flag to "not complete"


    lcd_writecommand(0x01);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);

    lcd_stringout("Initialized");
    for (unsigned int i = 0; i < 10; i++)
        {
            // lcd_moveto(2,0);
            // lcd_writebyte(i);
            _delay_ms(1000); 
            PWM_COUNTER++;
        }
    while (1)  // Loop forever
    {
        if(ADC_FLAG)
        {
            left_adc = ADC_VALUE; //read ADC value!
        }
        
    }             

	return 0;
}

