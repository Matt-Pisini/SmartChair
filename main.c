/***************************************************************

EE459 Final Project Main Source Code

Team Members: 	Matt Pisini, Jessica Mow, Zechen Zhou
Date: 			3/6/20
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
#include "lcd_strings.h"
#include "encoder.h"
#include "button.h"

/******************************************************* DEFINITIONS ******************************************************/

//serial definitions
#define CLK 7372800                                     //clock rate
#define BAUD 9600                                       //baud rate
#define UBBR ((CLK/16/BAUD)-1)                          //serial UBBR (should be 47)


//ADC definitions
static const char ADC_LCD_DISPLAY = 0;                  //flag for whether to display ADC values on LCD


//definitions from header file variables
volatile unsigned char ADC_COMPLETE_FLAG = 0;           //flag changed in ADC_vect when ADC is complete
volatile uint8_t new_adc_val_left, new_adc_val_right;   //stores new ADC value from ADCH register
volatile uint8_t old_adc_val_left, old_adc_val_right;   //stores old ADC value from ADCH register
volatile unsigned char ENCODER_VALUE;                   //maintains current value of encoder
volatile unsigned char BUTTON_FLAG;
volatile unsigned char new_encoder_state;
volatile unsigned char old_encoder_state;

//Global definitions
volatile unsigned char STATE;                           //stores value for the state machine
volatile unsigned char BUTTON_FLAG;                     //flag for whether button is pressed
volatile unsigned char LCD_CHANGE_FLAG;                 //flag to only write to LCD if something has changed



/**************** Pointers to state_array *****************/
/********** String values stored in lcd_strings.h *********/

//  State 0
PGM_P const state_0[] PROGMEM = 
                {
                    state0_string0, 
                    state0_string1, 
                    state0_string2, 
                    state0_string3
                }; 

const int STATE0_SIZE = (sizeof(state_0)/sizeof(state_0[0]));

//  State 1
PGM_P const state_1[] PROGMEM = 
                {   
                    state1_string0, 
                    state1_string1, 
                    state1_string2
                }; 

const int STATE1_SIZE = (sizeof(state_1)/sizeof(state_1[0]));

//  State 2

/***********************************************************/




int main( void )
{
    sei();                      //enable global interrupts

    /********************************* INITIALIZATIONS ********************************/

    lcd_init();                 //Initialize the LCD display
    servo_init();               //Initialize servo 
    timer_init();               //Initialize the PWM timer 
    ADC_init();                 //Initialize ADC
    adc_timer_init();           //Initialize the ADC timer
    button_init();
    encoder_init();
   
    /*********************************** DECLARATIONS *********************************/

    char buffer_left[4], buffer_right[4];   //buffer used to convert adc_val to string for LCD print


    /*********************************** DEFINITIONS *********************************/

    STATE = 0;                  //initialize to 'Splash Screen' state 0
    ENCODER_VALUE = 0;          //initialize encoder value to 0
    LCD_CHANGE_FLAG = 0;        //initialize as ready to display to lcd
    BUTTON_FLAG = 0;
    ENCODER_VALUE = 0;
    old_encoder_state = new_encoder_state;      //initialize encoder values to equal each other
    char prev_button_flag = 1;
    lcd_clear();
    // lcd_stringout("No Button");  
    char buf[4];


    while (1)  // Infinite loop
    {

/******************************** ROTARY ENCODER + BUTTON TEST **********************************/
        
        if (prev_button_flag != ENCODER_VALUE)
        {
            sprintf(buf, "%u", ENCODER_VALUE);        //convert byte into string of int value
            lcd_clear();
            lcd_moveto(0,0);
            lcd_stringout(buf);
            prev_button_flag = ENCODER_VALUE;
        }

        // if (prev_button_flag != BUTTON_FLAG)
        // {
        //     while ( BUTTON_FLAG )
        //     {
        //         lcd_clear();
        //         lcd_stringout("Button!");
        //         prev_button_flag = BUTTON_FLAG;

        //     }
        
        //     lcd_clear();
        //     lcd_stringout("No Button");
        //     BUTTON_FLAG = 0; 
        //     prev_button_flag = BUTTON_FLAG;
        // }
        




/******************************** ADC UPDATE & DISPLAY + PWM **********************************/

        if(ADC_COMPLETE_FLAG)                               //check if ADC complete
        {
            ADC_COMPLETE_FLAG = 0;                          //turn ADC flag off
            update_ADC(buffer_right, buffer_left);
        }
        if (ADC_LCD_DISPLAY)                                //ADC_LCD_DISPLAY = 1 if ADC values need to be displayed on LCD
        {
            display_ADC_LCD(buffer_right, buffer_left);
        }

        orient_servo(new_adc_val_left, new_adc_val_right);  //Orient servo
               
        
/************************************ FSM UPDATE & DISPLAY *************************************/
        
        if (LCD_CHANGE_FLAG)
        {
    
            switch(STATE) {

                case 0:

                    lcd_string_state_P(state_0, STATE0_SIZE, 0);
                    lcd_cursor(ENCODER_VALUE);
                    break;

                case 1:

                    lcd_string_state_P(state_1, STATE1_SIZE, 0);
                    lcd_cursor(ENCODER_VALUE);
                    break;

                case 2:

                    break;

                case 3:

                    break;

                case 4:

                    break;

                case 5:

                    break;

                case 6:

                    break;

            }
            LCD_CHANGE_FLAG = 0;
        }
    }            

/**********************************************************************************************/

	return 0;
}

