#include <avr/io.h>
#include <avr/interrupt.h>
#include "encoder.h"


void encoder_init()
{
	PCICR |= (1 << PCIE1);						//Pin change interrupt enable register 0
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);	//set pins for rotary encoders (PC0 & PC1)
	PORTC |= (1 << PC0 | 1 << PC1);     //enable pull-ups on PC4, PC5 for rotary encoder

}

ISR (PCINT1_vect)
{

	unsigned char bits, a, b; 						//variables for each of 2 PINC registers
	bits = PINC;									//read the two encoder inputs at the same time
	a = bits & (1 << PC0); 							//stores a as bit from PC0
	b = bits & (1 << PC1); 							//stores b as bit from PC1

	if ( old_encoder_state == 0)			//a stays 1
	{
		if (a)
		{
			//increment
			ENCODER_VALUE++;
			new_encoder_state = 1;
		}	
		else if (b)
		{
			//decrement
			ENCODER_VALUE--;
			new_encoder_state = 2;
		}
	}
	else if ( old_encoder_state == 1)		//a remains 0
	{
		if (!a)
		{
			//decrement
			ENCODER_VALUE--;
			new_encoder_state = 0;
		}	
		else if (b)
		{
			//increment
			ENCODER_VALUE++;
			new_encoder_state = 3;
		}
	}
	else if ( old_encoder_state == 2)		//b remains 1
	{
		if (a)
		{
			//decrement
			ENCODER_VALUE--;
			new_encoder_state = 3;
		}	
		else if (!b)
		{
			//increment
			ENCODER_VALUE++;
			new_encoder_state = 0;
		}		
	}
	else		//b remains 0
	{
		if (!a)
		{
			//increment
			ENCODER_VALUE++;
			new_encoder_state = 2;
		}	
		else if (!b)
		{
			//decrement
			ENCODER_VALUE--;
			new_encoder_state = 1;
		}			
	}

	old_encoder_state = new_encoder_state;					//reassign values

}