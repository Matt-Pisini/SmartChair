#include <avr/interrupt.h>


int int main(void)
{


	return 0;
}

void timer_init()
{
	TCNT1 = 0x1FF;
}

unsigned int TIM16_read( void )
{
	unsigned char sreg;
	unsigned int i;

	sreg = SREG;			//save global interrupt flag
	_CLI();					//disable interrupts
	i = TCNT1;

	SREG = sreg;			//restore global interrupt flag
	return i;

}