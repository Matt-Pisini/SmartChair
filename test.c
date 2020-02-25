#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "serial.h"

/*
serial_init - Initialize the USART port
*/
volatile uint8_t flag;

void timer_init()
{

	    // Set the Timer Mode to CTC
    TCNT1 |= (1 << WGM01);

    // Set the value that you want to count to
    OCR0A = 0x7F;

    TIMSK0 |= (1 << OCIE0A);    //Set the ISR COMPA vect

    sei();         //enable interrupts


    TCCR0B |= (1 << CS02);
    // set prescaler to 256 and start the timer

}

unsigned int TIM16_read( void )
{
	unsigned char sreg;
	unsigned int i;

	sreg = SREG;			//save global interrupt flag
	cli();					//disable interrupts
	i = TCNT1;

	SREG = sreg;			//restore global interrupt flag
	return i;

}

int main(void)
{
	timer_init();
	unsigned int timer;
	char buffer_input[10];
	char flag = 0;

	while(1)
	{
		if (flag)
		{
			timer = TIM16_read();
			memcpy(buffer_input, (char*)&timer, sizeof(int));
			// serial_out('a');
			string_out(buffer_input);
		}
		
		// input = serial_in();
		// printf("%c\n", input);
	}

	return 0;
}



ISR (TIMER0_COMPA_vect)  // timer0 overflow interrupt
{
    if (flag == 1)		//event to be exicuted every 4ms here
    {
    	flag = 0;
    }			
    else
    {
    	flag = 1;
    }
}

