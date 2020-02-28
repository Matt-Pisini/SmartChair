#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC.h"
#include "LCD.h"

// volatile uint8_t ADC_FLAG;

void ADC_init()
{

	ADMUX |= (1 << ADLAR);						//left adjust for 8 bit results
	ADMUX |= (1 << REFS0);						//AVCC for high voltage selection

	ADCSRA |= (1 << ADIE);						//ADC interrupt enable 
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);// | (1 << ADPS0);		//prescaler = 64
	
	// ADCSRA |= (1 << ADATE);						//sets autotrigger (controlled in ADCSRB)
	// ADCSRB |= (1 << ADTS2);						//timer/counter0 overflow interrupt trigger

}

void ADC_conversion(char ADC_MUX)
{
	ADMUX &= 0xF0;						//clears MUX bits
	ADMUX |= ADC_MUX;						//assigns select bits for correct ADC module
	ADCSRA |= (1 << ADEN);				//enables ADC module
	ADCSRA |= (1 << ADSC);				//initiates one conversion

}

//interrupt service routine
ISR (ADC_vect)
{

	ADC_FLAG = 1;								//set ADC flag when conversion complete
	ADCSRA &= ~(1 << ADEN); 					//disable ADC

}