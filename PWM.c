#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"

/********************************************* TIMER CODE FOR PWM ****************************************************************/

void timer_init()
{

		TCNT1 = 0;                              //initialize counter value to 0
		OCR1A = 1822;                           //CONT. MOTION SERVO CONTROL (based off ICR1 = 18400):
																						//No Motion           --> OCR1A  = (1820 - 1824)
																						//Full anticlockwise  --> OCR1A >= 1825
																						//Full clockwise      --> OCR1A <= 1819
		ICR1 = 18400;							              //18400 = 20 ms, 50Hz cycle

		TIMSK1 |= (1 << TOIE1) | (1 << OCIE1A); //enable overflow interrupt & CompareA interrupt

		TCCR1A |= (1 << WGM11);                 //sets to fast PWM mode using ICR1 as register to count to
		TCCR1B |= (1 << WGM12) | (1 << WGM13);

		TCCR1B |= (1 << CS11);                  //sets prescaler to 8 and starts timer
		PORTB |= (1 << SERVO_PORT); 
			
}

/********************************************************************************************************************************/


/********************************************* SERVO CODE FOR PWM ****************************************************************/

void init_servo()
{
	DDRB |= (1 << SERVO_PORT);                //sets PB2 to output 
}

/********************************************************************************************************************************/

/********************************************* ISR CODE FOR PWM ****************************************************************/

ISR (TIMER1_COMPA_vect)  // timer1 overflow interrupt service routine
{
	
	PORTB ^= (1 << SERVO_PORT);

}

ISR (TIMER1_OVF_vect)  // timer1 overflow interrupt service routine
{

	PORTB ^= (1 << SERVO_PORT);

}

/********************************************************************************************************************************/
