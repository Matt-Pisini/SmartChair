#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"

/********************************************* TIMER CODE FOR PWM ****************************************************************/

void timer_init()
{

		TCNT1 = 0;                              //initialize counter value to 0
		OCR1A = 1822;                           //CONT. MOTION SERVO CONTROL (based off ICR1 = 18400):
												// No Motion      --> OCR1A  = (1820 - 1824)
												// anticlockwise  --> OCR1A >= 1825
												// clockwise      --> OCR1A <= 1819
		ICR1 = 18400;							//18400 = 20 ms, 50Hz cycle

		TIMSK1 |= (1 << TOIE1) | (1 << OCIE1A); //enable overflow interrupt & CompareA interrupt

		TCCR1A |= (1 << WGM11);                 //sets to fast PWM mode using ICR1 as register to count to
		TCCR1B |= (1 << WGM12) | (1 << WGM13);

		TCCR1B |= (1 << CS11);                  //sets prescaler to 8 and starts timer
		PORTB |= (1 << SERVO_PORT); 
			
}


/********************************************* SERVO CODE FUNCTIONS ****************************************************************/

void servo_init()										//initialize servo
{
	DDRB |= (1 << SERVO_PORT);                			//sets PB2 to output 
}

/*	
	more light = lower ADC value
	increasing PWM_COUNTER moves anticlockwise (towards left)
	decreasing moves clockwise (towards right)
*/

void orient_servo(uint8_t left, uint8_t right)			//adjusts servo based on LDR values
{
    int error = right - left;							//calculate difference (aka error)

    if ( error > ADC_THRESHOLD_DIFF)                	//right has less light than left
    {
        if (PWM_COUNTER <= PWM_THRESHOLD_HIGH)			//stay below guard value
        {
            PWM_COUNTER++;
        }
    }

    else if ( error < (-1 * ADC_THRESHOLD_DIFF) )   	//right has more light than left
    {
        if (PWM_COUNTER >= PWM_THRESHOLD_LOW)			//stay above guard value
        {
            PWM_COUNTER--;
        }    
    }

    else                                               	//within threshold range (don't move)
    {
        PWM_COUNTER = 1822;
    }

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

ISR (TIMER0_OVF_vect){}

/********************************************************************************************************************************/
