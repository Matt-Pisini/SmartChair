#include <avr/io.h>
#include <avr/interrupt.h>
#include "button.h"

void button_init()
{
	PCICR |= (1 << PCIE1); 		//initializing interrupt register 1 for button
	PCMSK1 |= (1 << PCINT8); 	//setting the register mask to pins
}

ISR (PCINT1_vect)
{
	if ( PINB & (1 << PB7) )	//checks if button pushed
	{
		BUTTON_FLAG = 1;
	}
}