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
#include <stdlib.h>
#include "PWM.h"
#include "LCD.h"
#include "serial.h"
#include "ADC.h"
#include "lcd_strings.h"
#include "button_encoder.h"
#include "speech_module.h"
#include "uv_sensor.h"

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
volatile int8_t CURRENT_ENCODER_VAL;    		        //maintains current value of encoder
volatile int8_t PREV_ENCODER_VAL;
volatile unsigned char BUTTON_FLAG;                     //if button is pressed, flag goes high
volatile unsigned char LCD_CHANGE_FLAG;                 //flag to only write to LCD if something has changed

//Global definitions
volatile unsigned char CURRENT_STATE;                   //stores value for the state machine



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

// const int STATE0_SIZE = (sizeof(state_0)/sizeof(state_0[0]));
#define STATE0_SIZE (sizeof(state_0)/sizeof(state_0[0]))


//  State 1
PGM_P const state_1[] PROGMEM = 
				{   
					state1_string1, 
					state1_string2 
				}; 

#define STATE1_SIZE (sizeof(state_1)/sizeof(state_1[0]))

//  State 2
PGM_P const state_2[] PROGMEM = 
				{   
					state2_string1, 
					state2_string2, 
					state2_string3,
					state2_string4
				}; 

#define STATE2_SIZE (sizeof(state_2)/sizeof(state_2[0]))

//  State 3
PGM_P const state_3[] PROGMEM = 
				{   
					state3_string1, 
					state3_string2, 
					state3_string3,
					state3_string4,
					state3_string5
				}; 

#define STATE3_SIZE (sizeof(state_3)/sizeof(state_3[0]))

//  State 4
PGM_P const state_4[] PROGMEM = 
				{   
					state4_string1, 
					state4_string2, 
					state4_string3,
					state4_string4
				}; 

#define STATE4_SIZE (sizeof(state_4)/sizeof(state_4[0]))

//  State 5
PGM_P const state_5[] PROGMEM = 
				{   
					state5_string1, 
					state5_string2, 
					state5_string3,
					state5_string4
				}; 

#define STATE5_SIZE (sizeof(state_5)/sizeof(state_5[0]))

//  State 6
PGM_P const state_6[] PROGMEM = 
				{   
					state6_string1, 
					state6_string2, 
					state6_string3,
					state6_string4
				}; 

#define STATE6_SIZE (sizeof(state_6)/sizeof(state_6[0]))

//  State 7
PGM_P const state_7[] PROGMEM = 
				{   
					state7_string1, 
					state7_string2
				}; 

#define STATE7_SIZE (sizeof(state_7)/sizeof(state_7[0]))

//  State 8
PGM_P const state_8[] PROGMEM = 
				{   
					state8_string1, 
					state8_string2, 
					state8_string3,
					state8_string4
				}; 

#define STATE8_SIZE (sizeof(state_8)/sizeof(state_8[0]))

//  State 9
PGM_P const state_9[] PROGMEM = 
				{   
					state9_string1, 
					state9_string2, 
					state9_string3,
					state9_string4
				}; 

#define STATE9_SIZE (sizeof(state_9)/sizeof(state_9[0]))

//  State 10
PGM_P const state_10[] PROGMEM = 
				{   
					state10_string1, 
					state10_string2, 
					state10_string3,
					state10_string4
				}; 

#define STATE10_SIZE (sizeof(state_10)/sizeof(state_10[0]))

const int STATE_ARRAY_SIZES[] = {
	STATE0_SIZE, STATE1_SIZE, STATE2_SIZE, STATE3_SIZE, STATE4_SIZE,
	STATE5_SIZE, STATE6_SIZE, STATE7_SIZE, STATE8_SIZE, STATE9_SIZE,
	STATE10_SIZE
};
#define STATES (sizeof(STATE_ARRAY_SIZES)/sizeof(STATE_ARRAY_SIZES[0]))									//number of states for LCD

/**************** State Transition Table *****************/

const int8_t *state_transition_table[] = {
	&state0_transitions[0],
	&state1_transitions[0],
	&state2_transitions[0],
	&state3_transitions[0],
	&state4_transitions[0],
	&state5_transitions[0],
	&state6_transitions[0],
	&state7_transitions[0],
	&state8_transitions[0],
	&state9_transitions[0],
	&state10_transitions[0]
};

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
	i2c_init();					//Initialization of I2C
   	serial_init(UBBR);			//Initialization of asyn. serial for speech module

	/*********************************** DECLARATIONS *********************************/

	char buffer_left[4], buffer_right[4];   //buffer used to convert adc_val to string for LCD print


	/*********************************** DEFINITIONS *********************************/

	CURRENT_STATE = 1;                  //initialize to 'Splash Screen' state 0
	CURRENT_ENCODER_VAL = 0;          		//initialize encoder value to 0
	PREV_ENCODER_VAL = 0;
	LCD_CHANGE_FLAG = 1;        		//initialize as ready to display to lcd
	BUTTON_FLAG = 0;
	uint8_t DISPLAY_INDEX = 0;			//inidicates what lines of state will be displayed (i.e. if state has 4+ lines of string)
	uint8_t CURSOR_VAL = 0;					//where the cursor is to be displayed (row 0 -4 on LCD)
	int8_t VALID_MOVE = 0;
	uint8_t MAX_SIZE = 0;
	uint8_t CURRENT_INDEX = 0;
	uint8_t i;	


	//Splash screen: displays for 3 seconds
	lcd_clear();
	lcd_string_state_P(state_0, STATE0_SIZE, 0);
	lcd_writecommand(0x0c);						//turn cursor off
	_delay_ms(3000);							
	lcd_writecommand(0x0f);						//turn cursor on


	while (1)  // Infinite loop
	{

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

/***************** ENCODER USED: cursor change *******************/
			
			//encoder increase
			if (CURRENT_ENCODER_VAL > PREV_ENCODER_VAL)
			{

				MAX_SIZE = STATE_ARRAY_SIZES[CURRENT_STATE] - 1;
				CURRENT_INDEX = CURSOR_VAL + DISPLAY_INDEX;

				//cursor at upper boundary
				if (CURRENT_INDEX == MAX_SIZE)
				{
					//cursor stays on MAX
				}
				//cursor at upper boundary && implicitly room to go
				else if(CURSOR_VAL == 3)
				{
					VALID_MOVE = state_transition_table[CURRENT_STATE][CURRENT_INDEX + 1];
					DISPLAY_INDEX++;

					//invalid cursor display
					if (VALID_MOVE == -1)
					{
						CURSOR_VAL--;
					}
					//else: valid cursor display --> cursor stays on 3
				}
				//cursor in the middle
				else
				{
					CURSOR_VAL++;
				}
			}
			//encoder decrease
			else if (CURRENT_ENCODER_VAL < PREV_ENCODER_VAL)
			{
				CURRENT_INDEX = CURSOR_VAL + DISPLAY_INDEX;
				VALID_MOVE = state_transition_table[CURRENT_STATE][CURRENT_INDEX - 1];
				
				//cursor at lower boundary
				if (CURSOR_VAL == 0)
				{
					//lower boundary: more screen to show
					if (DISPLAY_INDEX)
					{
						
						//valid cursor display
						if (VALID_MOVE != -1)
						{
							DISPLAY_INDEX--;
						}

						//invalid cursor display
						else
						{
							DISPLAY_INDEX--;
							CURSOR_VAL++;
						}
						
					}
					//else: lower boundary & no more screen to show --> cursor values stays the same
				}
				//cursor in the middle
				else
				{
					//middle cursor: invalid cursor display
					if (VALID_MOVE == -1)
					{
						if (DISPLAY_INDEX)
						{
							DISPLAY_INDEX--;
							CURSOR_VAL++;
						}
						//else: cursor value doesn't change
					}
					//middle cursor: valid cursor display
					else
					{
						CURSOR_VAL--;
					}
					
				}			
			}
			/*
			else 
			{
				encoder hasnt changed
			}
			*/
			

/***************** BUTTON USED: state transition *******************/

			if (BUTTON_FLAG)
			{
				BUTTON_FLAG = 0;
				CURRENT_STATE = state_transition_table[CURRENT_STATE][CURSOR_VAL + DISPLAY_INDEX];
				
				//Finds first valid postion the cursor can be at in next state
				i = 0;
				CURSOR_VAL = 0;
				while(state_transition_table[CURRENT_STATE][i] == -1)
				{

					i++;
					CURSOR_VAL = i;

				}
				//resets display index
				DISPLAY_INDEX = 0;
			}



			
			switch(CURRENT_STATE) {

				//for debugging
				//lcd_moveto(3, 15);
				//sprintf(buf, "%d", VALID_MOVE);
				//lcd_stringout(buf);
				case 0:
					lcd_clear();
					lcd_string_state_P(state_0, STATE0_SIZE, 0);
					// lcd_cursor(CURRENT_ENCODER_VAL);

					break;

				case 1:

					lcd_clear();
					lcd_string_state_P(state_1, STATE1_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 2:

					lcd_clear();
					lcd_string_state_P(state_2, STATE2_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 3:
	
					lcd_clear();			
					lcd_string_state_P(state_3, STATE3_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);
		
					break;

				case 4:

					lcd_clear();
					lcd_string_state_P(state_4, STATE4_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 5:
	
					lcd_clear();
					lcd_string_state_P(state_5, STATE5_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 6:

					lcd_clear();
					lcd_string_state_P(state_6, STATE6_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 7:

					lcd_clear();
					lcd_string_state_P(state_7, STATE7_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 8:

					lcd_clear();
					lcd_string_state_P(state_8, STATE8_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 9:

					lcd_clear();
					lcd_string_state_P(state_9, STATE9_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;

				case 10:

					lcd_clear();
					lcd_string_state_P(state_10, STATE10_SIZE, DISPLAY_INDEX);
					lcd_cursor(CURSOR_VAL);

					break;


			}
			//ensure no overflow of encoder values
			PREV_ENCODER_VAL = 0;
			CURRENT_ENCODER_VAL = 0;
			LCD_CHANGE_FLAG = 0;
		}
	}            

/**********************************************************************************************/

	return 0;
}

