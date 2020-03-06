#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "encoder.h"

void encoder_init()
{
	PCICR |= (1 << PCIE0);						//Pin change interrupt enable
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);	//set pins for rotary encoders (PC0 & PC1)
}

ISR (PCINT0_vect)
{

	unsigned char bits, a, b; 						//variables for each of 2 PINC registers
	bits = PINC;									//read the two encoder inputs at the same time
	a = bits & (1 << PC0); 							//stores a as bit from PC0
	b = bits & (1 << PC1); 							//stores b as bit from PC1

	//change ENCODER_VALUE
}