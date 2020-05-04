#ifndef BUTTON_ENCODER_MODULE

/**************************** GLOBAL VARIABLES ***************************/
volatile int8_t CURRENT_ENCODER_VAL;									//maintains current value of encoder
volatile unsigned char BUTTON_FLAG;										//Button has been pressed
volatile unsigned char button_timer_flag;								//Triggers debouncing timer from button interrupt
volatile unsigned char LCD_CHANGE_FLAG;                					//flag to indicate some LCD aspect has changed

volatile unsigned char bits, a, b; 										//variables for each of 2 PINC registers (rotary encoders)

volatile unsigned char old_new_state;									//4-bit value used to index state transition table (rot_enc_table)
volatile unsigned char value;											//Used to capture two 4-bit values of old_state_new_state into 1 register.
																		//The end of any debouncing sequence has 1 unique value for CW (0x17) and another for CCW(0x2b) 
static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};		//Table of valid state transitions for encoder.
																		//Possible transitions from prev state to a new state captured in form of 4-bit table.


/**************************** FUNCTIONS ***************************/
void encoder_init(); 
void button_init();

#define BUTTON_ENCODER_MODULE
#endif 





