#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"

void ADC_init()
{

	ADMUX |= (1 << ADLAR);						//left adjust for 8 bit results
	ADMUX |= (1 << REFS0);						//AVCC for high voltage selection

	ADCSRA |= (1 << ADIE);						//ADC interrupt enable 
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);		//prescaler = 64
	// ADCSRA |= (1 << ADATE);						//sets autotrigger (controlled in ADCSRB)

	ADCSRB |= (1 << ADTS2);						//timer/counter0 overflow interrupt trigger

}


void ADC_conversion(char LDR)
{

	ADMUX |= (1 << LDR);						//mux selection for photoresistor
	ADCSRA |= (1 << ADCEN); 					//enable ADC

}

//interrupt service routine
ISR (ADC_vect)
{

	ADC_FLAG = 1;								//set ADC flag when conversion complete
	ADCSRA &= ~(1 << ADCEN); 					//disable ADC

}