/***************************************************************

EE459 Final Project Main Source Code

Team Members: 	Matt Pisini, Jessica Mow, Zechen Zhou
Date: 			3/1/20
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
#define ADC_LCD_DISPLAY ((char) 0)              //flag for whether to display ADC values on LCD


//function definitions
void update_ADC(char *buf_right, char *buf_left);
void display_ADC_LCD(char *buf_right, char *buf_left);


//definitions from header file variables
volatile uint8_t ADC_COMPLETE_FLAG = 0;

//Global definitions
uint8_t new_adc_val_left, new_adc_val_right;            //stores new ADC value from ADCH register
uint8_t old_adc_val_left, old_adc_val_right;            //stores old ADC value from ADCH register


            /********************************* LCD STRINGS (FLASH MEM.) ********************************/

                const unsigned char str1[] PROGMEM = "Very nice";

            /********************************************************************************************/


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

    char buffer_left[4], buffer_right[4];                   //buffer used to convert adc_val to string for LCD print

    /*********************************************************************************/

    /*********************************** DEFINITIONS *********************************/

    /*********************************************************************************/


    for (int i = 0; i < 10; i++)
    {
        lcd_clear();
        lcd_moveto(0,0);
        lcd_stringout("yeeet: ");
        _delay_ms(250);
        lcd_row_blink(0);
        _delay_ms(250);
    }
    // lcd_stringout_P((char *) str1);
    _delay_ms(1000);


    while (1)  // Infinite loop
    {
        orient_servo(new_adc_val_left, new_adc_val_right);              //Orient servo

/************************************ ADC UPDATE & DISPLAY *************************************/


        if(ADC_COMPLETE_FLAG)                                           //check if ADC complete
        {
            ADC_COMPLETE_FLAG = 0;                                      //turn ADC flag off
            update_ADC(buffer_right, buffer_left);
        }
        if (ADC_LCD_DISPLAY)                                        //ADC_LCD_DISPLAY = 1 if ADC values need to be displayed on LCD
        {
            display_ADC_LCD(buffer_right, buffer_left);
        }
               
/**********************************************************************************************/
        

    }            

	return 0;
}



/******************************************** FUNCTIONS *********************************************/

void display_ADC_LCD(char *buf_right, char *buf_left)           //displays ADC values to LCD
{
    if (new_adc_val_right != old_adc_val_right) 
    {
        lcd_writecommand(1);
        lcd_moveto(0, 0);
        lcd_stringout("RIGHT_LDR: ");
        lcd_stringout(buf_right);
        lcd_moveto(1, 0);
        lcd_stringout("LEFT_LDR: ");
        lcd_stringout(buf_left);
        old_adc_val_right = new_adc_val_right;                  //update value for comparison
    }
    if (new_adc_val_left != old_adc_val_left)
    {
        lcd_writecommand(1);
        lcd_moveto(0, 0);
        lcd_stringout("RIGHT_LDR: ");
        lcd_stringout(buf_right);
        lcd_moveto(1, 0);
        lcd_stringout("LEFT_LDR: ");
        lcd_stringout(buf_left);
        old_adc_val_left = new_adc_val_left;                    //update value for comparison
    }
}


void update_ADC(char *buf_right, char *buf_left)                //updates ADC value when ready
{
    if ((ADMUX & 0x0F) == RIGHT_LDR)                            //check if RIGHT_LDR mux value
    {
        new_adc_val_right = ADC_VALUE;
        ADMUX &= 0xF0;                                          //clears MUX bits for ADC        
        ADMUX |= LEFT_LDR;                                      //assigns select bits for next ADC measurement (LEFT_LDR)
        buf_right[0] = '\0';                                    //clear buffers
        sprintf(buf_right, "%u", new_adc_val_right);        //convert byte into string of int value
    }
    else
    {
        new_adc_val_left = ADC_VALUE;
        ADMUX &= 0xF0;                                          //clears MUX bits for ADC
        ADMUX |= RIGHT_LDR;                                     //assigns select bits for next ADC measurement (RIGHT_LDR)  
        buf_left[0] = '\0';                                     //clear buffers
        sprintf(buf_left, "%u", new_adc_val_left);          //convert byte into string of int value
    }
}

/****************************************************************************************************/
