#include <avr/io.h>
#include <avr/interrupt.h>
#include "button.h"


void button_init()
{
	PCICR |= (1 << PCIE0); 		//initializing interrupt register B for button
	PCMSK0 |= (1 << PCINT7); 	//setting the register mask to pins
}

ISR (PCINT0_vect)
{

	if ( PINB & (1 << PB7) )	//checks if button pushed
	{
		BUTTON_FLAG++;
	}

}