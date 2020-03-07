#ifndef ENCODER_MODULE

/**************************** GLOBAL VARIABLES ***************************/
volatile unsigned char ENCODER_VALUE;		//maintains current value of encoder
volatile unsigned char encoder_a;
volatile unsigned char encoder_b;

/**************************** FUNCTIONS ***************************/
void encoder_init(); 

#define ENCODER_MODULE
#endif