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

/******************************************************* DEFINITIONS ******************************************************/

//serial definitions
#define CLK 7372800                             //clock rate
#define BAUD 9600                               //baud rate
#define UBBR ((CLK/16/BAUD)-1)                  //serial UBBR (should be 47)


//ADC definitions
#define ADC_VALUE ADCH                          //register where 8-bit ADC value kept
#define LEFT_LDR ((char)0x02)                   //left LDR pin on board. Corresponds to PC2 (ADC2)
#define RIGHT_LDR ((char)0x03)                  //right LDR pin on board. Corresponds to PC3(ADC3)


//function definitions


//definitions from header file variables
volatile uint8_t ADC_COMPLETE_FLAG = 0;

/**************************************************************************************************************************/


int main( void )
{
    sei();                      //enable global interrupts

    /********************************* INITIALIZATIONS ********************************/

    lcd_init();                 //Initialize the LCD display
    servo_init();               //Initialize servo 
    timer_init();               //Initialize the PWM timer 
    ADC_init();                 //Initialize ADC
    adc_timer_init();           //Initialize the ADC timer

    /*********************************************************************************/
   
    /*********************************** DECLARATIONS *********************************/

    uint8_t old_adc_val_left, old_adc_val_right;            //stores old ADC value from ADCH register
    uint8_t new_adc_val_left, new_adc_val_right;            //stores new ADC value from ADCH register
    char buffer_left[4], buffer_right[4];                   //buffer used to convert adc_val to string for print
    uint8_t n;                                              //stores number of last element in buffer
    char ADC_LCD_DISPLAY;                                   //flag for whether to display ADC values on LCD


    /*********************************************************************************/

    /*********************************** DEFINITIONS *********************************/

    old_adc_val_left = 0;
    old_adc_val_right = 0;
    new_adc_val_left = 0;
    new_adc_val_right = 0;
    ADC_LCD_DISPLAY = 1;

    /*********************************************************************************/

    lcd_writecommand(1);
    lcd_moveto(0, 0);

    lcd_stringout("Starting");
    lcd_moveto(1,0);
    _delay_ms(1000);


    while (1)  // Loop forever
    {


/************************************ ADC calculations *************************************/

        orient_servo(new_adc_val_left, new_adc_val_right);              //Correct servo orientation

        if(ADC_COMPLETE_FLAG)                                           //check if ADC complete
        {

            ADC_COMPLETE_FLAG = 0;                                      //turn ADC flag off

            if ((ADMUX & 0x0F) == RIGHT_LDR)                            //check if RIGHT_LDR mux value
            {

                new_adc_val_right = ADC_VALUE;                          //read in new ADC value for RIGHT_LDR
                ADMUX &= 0xF0;                                          //clears MUX bits for ADC
                ADMUX |= LEFT_LDR;                                      //assigns select bits for next ADC measurement (LEFT_LDR)
                buffer_right[0] = '\0';
                n = sprintf(buffer_right, "%u", new_adc_val_right);     //convert byte into string of int value
                buffer_right[n] = '\0';                                 //append null terminator

            }
            else                                                        //else LEFT_LDR mux value
            {

                new_adc_val_left = ADC_VALUE;                           //read in new ADC value for LEFT_LDR
                ADMUX &= 0xF0;                                          //clears MUX bits for ADC
                ADMUX |= RIGHT_LDR;                                     //assigns select bits for next ADC measurement (RIGHT_LDR)
                buffer_left[0] = '\0';                                  //clear buffers
                n = sprintf(buffer_left, "%u", new_adc_val_left);       //convert byte into string of int value
                buffer_left[n] = '\0';                                  //append null terminator
            
            }

            if (ADC_LCD_DISPLAY)                                        //ADC_LCD_DISPLAY = 1 if ADC values need to be displayed on LCD
            {
                if (new_adc_val_right != old_adc_val_right) 
                {
                    lcd_writecommand(1);
                    lcd_moveto(0, 0);
                    lcd_stringout("RIGHT_LDR: ");
                    lcd_stringout(buffer_right);
                    lcd_moveto(1, 0);
                    lcd_stringout("LEFT_LDR: ");
                    lcd_stringout(buffer_left);
                    old_adc_val_right = new_adc_val_right;              //update value for comparison

                }
                if (new_adc_val_left != old_adc_val_left)
                {
                    lcd_writecommand(1);
                    lcd_moveto(0, 0);
                    lcd_stringout("RIGHT_LDR: ");
                    lcd_stringout(buffer_right);
                    lcd_moveto(1, 0);
                    lcd_stringout("LEFT_LDR: ");
                    lcd_stringout(buffer_left);
                    old_adc_val_left = new_adc_val_left;                //update value for comparison

                }
            }

        }        

/**********************************************************************************************/
        

    }            

	return 0;
}


/******************************************** FUNCTIONS *********************************************/



/****************************************************************************************************/
