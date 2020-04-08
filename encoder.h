#ifndef ENCODER_MODULE

/**************************** GLOBAL VARIABLES ***************************/
volatile unsigned char ENCODER_VALUE;		//maintains current value of encoder
volatile unsigned char new_encoder_state;
volatile unsigned char old_encoder_state;

/**************************** FUNCTIONS ***************************/
void encoder_init(); 

#define ENCODER_MODULE
#endif