#include <avr/io.h>
#include <avr/interrupt.h>
#include "button_encoder.h"


/*

	With assistance for debouncing component from: https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders
	- Used the valid state transition table for rotary encoder
	- Used the logic for comparison to indicate the end of a bouncing sequence

*/


void button_init()
{
	PCICR |= (1 << PCIE0); 		//initializing interrupt register B for button
	PCMSK0 |= (1 << PCINT7); 	//setting the register mask to pins
}

void encoder_init()
{
	PCICR |= (1 << PCIE1);						//Pin change interrupt 1 register enabled
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);	//set pins for rotary encoders (PC0 & PC1) on pin change mask register
	PORTC |= (1 << PC0 | 1 << PC1);     		//enable pull-ups on PC0, PC1 for rotary encoder
	old_new_state = 0;
	value = 0;
}

void debouncer_init()
{

	//TIMER 2 (8-bit) --> normal operation.
	//overflow occurs in ~4.4ms where interrupt is triggered.

	TIMSK2 |= (1 << TOIE2);						//overflow interrupt enabled
	TIFR2 |= (1 << TOV2);						//overflow flag — determines which comaparison based interrupt vector used

}

ISR (PCINT0_vect)
{

	PCMSK0 &= ~(1 << PCINT7);				//disables PB7 as interrupt source until debounce timer finishes
	button_timer_flag = 1;					//sets timer flag from bounce interrupt
	TCCR2B |= (1<< CS22) | (1 << CS20);		//sets debouncer timer (prescaler = 128)

}

ISR (PCINT1_vect)
{

	bits = PINC;								//read the two encoder inputs at the same time
	a = bits & (1 << PC0); 						//stores a as bit from PC0
	b = bits & (1 << PC1); 						//stores b as bit from PC1
	
	old_new_state <<= 2;						//shift old 2-bit encoder state to the left
	old_new_state |= (a | b);					//add new encoder inputs to old_new_state
	old_new_state &= 0x0f;						//clear 4 MSB's (only care about LSB 4-bit value for rot_enc_table)

	if (rot_enc_table[old_new_state])			//check to see if old_new_state, which is combination of previous encoder 
	{											//state and current encoder state, is a valid state transition in table

		value <<=4;								//If it is a valid state (only at beginning or end of debouncing sequence), shift 4 bits for comparison
		value |= old_new_state;					//insert current 4-bit combo of prev. state and current state
		
		if ( (value & 0xff) == 0x2b)			//This is always ending sequence for encoder bounce (CCW) 
		{
			ENCODER_VALUE--;					//decrement encoder
		}
		
		if ( (value & 0xff) == 0x17)			//This is always ending sequence for encoder bounce (CW)
		{
			ENCODER_VALUE++;					//increment encoder
		}
	}
}

/**************************** DEBOUNCING TIMER *******************************/


ISR (TIMER2_OVF_vect)
{
 
	if (button_timer_flag)				//if PCINT7 is not currently set
	{
		if ( PINB & (1 << PB7) )		//checks if button pushed
		{
			BUTTON_FLAG = 1;				//turn on button flag
		}

		PCMSK0 |= (1 << PCINT7);		//re-enables interrupts on button input
		button_timer_flag = 0;			//resets button flag

	}

	TCCR2B &= 0xF8;						//clear prescaler to disable timer

}

