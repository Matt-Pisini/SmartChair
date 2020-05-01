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
#include "button_encoder.h"

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
volatile unsigned char BUTTON_FLAG;                     //if button is pressed, flag goes high
volatile unsigned char LCD_CHANGE_FLAG;                 //flag to only write to LCD if something has changed

//Global definitions
volatile unsigned char CURRENT_STATE;                   //stores value for the state machine
volatile unsigned char PREV_STATE;						//stores value of previous state



/**************** Pointers to state_array *****************/
/********** String values stored in lcd_strings.h *********/

//  State 0
PGM_P const state_0[] PROGMEM = 
				{
					state0_string1, 
					state0_string2, 
					state0_string3, 
					state0_string4
				}; 

const int STATE0_SIZE = (sizeof(state_0)/sizeof(state_0[0]));

//  State 1
PGM_P const state_1[] PROGMEM = 
				{   
					state1_string1, 
					state1_string2 
				}; 

const int STATE1_SIZE = (sizeof(state_1)/sizeof(state_1[0]));

//  State 2
PGM_P const state_2[] PROGMEM = 
				{   
					state2_string1, 
					state2_string2, 
					state2_string3,
					state2_string4
				}; 

const int STATE2_SIZE = (sizeof(state_2)/sizeof(state_2[0]));

//  State 3
PGM_P const state_3[] PROGMEM = 
				{   
					state3_string1, 
					state3_string2, 
					state3_string3,
					state3_string4,
					state3_string5
				}; 

const int STATE3_SIZE = (sizeof(state_3)/sizeof(state_3[0]));

//  State 4
PGM_P const state_4[] PROGMEM = 
				{   
					state4_string1, 
					state4_string2, 
					state4_string3,
					state4_string4
				}; 

const int STATE4_SIZE = (sizeof(state_4)/sizeof(state_4[0]));

//  State 5
PGM_P const state_5[] PROGMEM = 
				{   
					state5_string1, 
					state5_string2, 
					state5_string3,
					state5_string4
				}; 

const int STATE5_SIZE = (sizeof(state_5)/sizeof(state_5[0]));

//  State 6
PGM_P const state_6[] PROGMEM = 
				{   
					state6_string1, 
					state6_string2, 
					state6_string3,
					state6_string4
				}; 

const int STATE6_SIZE = (sizeof(state_6)/sizeof(state_6[0]));

//  State 7
PGM_P const state_7[] PROGMEM = 
				{   
					state7_string1, 
					state7_string2
				}; 

const int STATE7_SIZE = (sizeof(state_7)/sizeof(state_7[0]));

//  State 8
PGM_P const state_8[] PROGMEM = 
				{   
					state8_string1, 
					state8_string2, 
					state8_string3,
					state8_string4
				}; 

const int STATE8_SIZE = (sizeof(state_8)/sizeof(state_8[0]));

//  State 9
PGM_P const state_9[] PROGMEM = 
				{   
					state9_string1, 
					state9_string2, 
					state9_string3,
					state9_string4
				}; 

const int STATE9_SIZE = (sizeof(state_9)/sizeof(state_9[0]));

//  State 10
PGM_P const state_10[] PROGMEM = 
				{   
					state10_string1, 
					state10_string2, 
					state10_string3,
					state10_string4
				}; 

const int STATE10_SIZE = (sizeof(state_10)/sizeof(state_10[0]));

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
	button_init();              //intitialize button as interrupt on PB7
	encoder_init();             //Initialize rotary encoder on PC0 & PC1
   
	/*********************************** DECLARATIONS *********************************/

	char buffer_left[4], buffer_right[4];   //buffer used to convert adc_val to string for LCD print


	/*********************************** DEFINITIONS *********************************/

	CURRENT_STATE = 0;                  //initialize to 'Splash Screen' state 0
	PREV_STATE = 1;						//initialize previous state value
	ENCODER_VALUE = 0;          		//initialize encoder value to 0
	LCD_CHANGE_FLAG = 1;        		//initialize as ready to display to lcd
	BUTTON_FLAG = 0;
	lcd_clear();


	while (1)  // Infinite loop
	{

/******************************** ROTARY ENCODER + BUTTON TEST **********************************/
		
		/*
		if (prev_button_flag != BUTTON_FLAG)
		{
			sprintf(buf, "%u", BUTTON_FLAG);        //convert byte into string of int value
			lcd_clear();
			lcd_moveto(0,0);
			lcd_stringout(buf);
			prev_button_flag = BUTTON_FLAG;
		}
		*/

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
	
			switch(CURRENT_STATE) {

				case 0:

					if(CURRENT_STATE != PREV_STATE)
					{
						lcd_string_state_P(state_0, STATE0_SIZE, 0);
					}
					lcd_cursor(ENCODER_VALUE);

					break;

				case 1:
					if(CURRENT_STATE != PREV_STATE)
					{
						lcd_string_state_P(state_1, STATE1_SIZE, 0);
					}
					lcd_cursor(ENCODER_VALUE);
					break;

				case 2:

					lcd_string_state_P(state_2, STATE2_SIZE, 0);

					break;

				case 3:

					lcd_string_state_P(state_3, STATE3_SIZE, 0);

					break;

				case 4:

					lcd_string_state_P(state_4, STATE4_SIZE, 0);

					break;

				case 5:

					lcd_string_state_P(state_5, STATE5_SIZE, 0);

					break;

				case 6:

					lcd_string_state_P(state_6, STATE6_SIZE, 0);

					break;

				case 7:

					lcd_string_state_P(state_7, STATE7_SIZE, 0);

					break;

				case 8:

					lcd_string_state_P(state_8, STATE8_SIZE, 0);

					break;

				case 9:

					lcd_string_state_P(state_9, STATE9_SIZE, 0);

					break;

				case 10:

					lcd_string_state_P(state_10, STATE10_SIZE, 0);

					break;


			}
			PREV_STATE = CURRENT_STATE;
			LCD_CHANGE_FLAG = 0;
		}
	}            

/**********************************************************************************************/

	return 0;
}

