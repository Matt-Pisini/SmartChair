#include <avr/io.h>
#include <avr/interrupt.h>
#include "encoder.h"

void encoder_init()
{
	PCICR |= (1 << PCIE0);						//Pin change interrupt enable register 0
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);	//set pins for rotary encoders (PC0 & PC1)
	encoder_a = PINC & (1 << PC0);
	encoder_b = PINC & (1 << PC1);
}

ISR (PCINT0_vect)
{

	unsigned char bits, a, b; 						//variables for each of 2 PINC registers
	bits = PINC;									//read the two encoder inputs at the same time
	a = bits & (1 << PC0); 							//stores a as bit from PC0
	b = bits & (1 << PC1); 							//stores b as bit from PC1

	if ( (a == encoder_a) & a)			//a stays 1
	{
		if (b > encoder_b)
		{
			//increment
		}	
		else
		{
			//decrement
		}
	}
	else if ( (a == encoder_a) & !a)		//a remains 0
	{
		if (b > encoder_b)
		{
			//decrement
		}	
		else
		{
			//increment
		}
	}
	else if ( (b == encoder_b) & b)		//b remains 1
	{
		if (a > encoder_a)
		{
			//decrement
		}	
		else
		{
			//increment
		}		
	}
	else if ( (b == encoder_b) & !b)		//b remains 0
	{
		if (a > encoder_a)
		{
			//increment
		}	
		else
		{
			//decrement
		}			
	}

	encoder_a = a;						//reassign values
	encoder_b = b;

}