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

#define BAUD_RATE ((unsigned short) 47)
#define PWM_COUNTER OCR1A       //increase/decrease to move servo
#define ADC_VALUE ADCH          //register where 8-bit ADC value kept

#define CLK 7372800
#define BAUD 9600
#define UBBR ((CLK/16/BAUD)-1)


volatile uint8_t ADC_FLAG = 0;
volatile char LEFT_LDR = 0x02;

#define LEFT_LDR ((char)0x02)       //left LDR pin on board. Corresponds to PC2 (ADC2)
#define RIGHT_LDR ((char)0x03)      //right LDR pin on board. Corresponds to PC3(ADC3)

int main( void )
{
    sei();                      //enable global interrupts

    /**************** INITIALIZATIONS ***************/

    lcd_init();                 //Initialize the LCD display
    timer_init();               //Initialize the PWM timer 
    init_servo();               //Initialize servo 
    ADC_init();
   
    uint8_t adc_val;            //stores ADC value from ADCH register
    char buffer[5];            //buffer used to convert adc_val to string for print
    uint8_t n;                  //stores number of last element in buffer

    lcd_writecommand(0x01);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);
    _delay_ms(1000);

    // for (unsigned int i = 0; i < 10; i++)
    //     {
    //         // lcd_moveto(2,0);
    //         // lcd_writebyte(i);
    //         _delay_ms(1000); 
    //         PWM_COUNTER++;
    //     }

    while (1)  // Loop forever
    {
        if (!ADC_FLAG)
        {
            ADC_conversion(LEFT_LDR);
        }

        if(ADC_FLAG)
        {
            adc_val = ADC_VALUE;                    //read ADC value!
            ADC_FLAG = 0;                           //turn ADC flag off
            n = sprintf(buffer, "%u", adc_val);     //convert byte into string int
            buffer[n] = '\0';                       //append null terminator
            lcd_writecommand(0x01);
            lcd_moveto(0, 0);
            lcd_stringout(buffer);
            buffer[0] = '\0';                       //clear buffer
            _delay_ms(100);
        }
        
    }             

	return 0;
}

