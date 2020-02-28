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
#define PWM_COUNTER OCR1A                   //increase/decrease to move servo
#define ADC_VALUE ADCH                      //register where 8-bit ADC value kept

#define CLK 7372800                         //clock rate
#define BAUD 9600                           //baud rate
#define UBBR ((CLK/16/BAUD)-1)              //serial UBBR


//definitions of previosuly declared header file variables
volatile uint8_t ADC_FLAG = 0;


#define LEFT_LDR ((char)0x02)       //left LDR pin on board. Corresponds to PC2 (ADC2)
#define RIGHT_LDR ((char)0x03)      //right LDR pin on board. Corresponds to PC3(ADC3)

int main( void )
{
    sei();                      //enable global interrupts

    /******************************** INITIALIZATIONS *****************************/

    lcd_init();                 //Initialize the LCD display
    init_servo();               //Initialize servo 
    timer_init();               //Initialize the PWM timer 
    ADC_init();                 //Initialize ADC

    /*****************************************************************************/
   
    uint8_t adc_val;            //stores ADC value from ADCH register
    char buffer[4];             //buffer used to convert adc_val to string for print
    uint8_t n;                  //stores number of last element in buffer

    lcd_writecommand(0x01);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);
    _delay_ms(1000);

    // for (unsigned int i = 0; i < 10; i++)
    //     {
    //         _delay_ms(1000); 
    //         PWM_COUNTER++;
    //     }

    while (1)  // Loop forever
    {


/************************************ ADC calculations *************************************/

        if (!ADC_FLAG)
        {
            ADC_conversion(LEFT_LDR);
        }

        if(ADC_FLAG)
        {
            adc_val = ADC_VALUE;                    //read ADC value!
            ADC_FLAG = 0;                           //turn ADC flag off
            n = sprintf(buffer, "%u", adc_val);     //convert byte into string of int value
            buffer[n] = '\0';                       //append null terminator
            lcd_writecommand(0x01);
            lcd_moveto(0, 0);
            lcd_stringout(buffer);
            buffer[0] = '\0';                       //clear buffer
            _delay_ms(100);
        }
/**********************************************************************************************/
        

    }             

	return 0;
}

