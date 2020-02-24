#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

/*
serial_init - Initialize the USART port
*/

void serial_init(unsigned short ubrr) {

	UBRR0 = ubrr;				// Set baud rate
	UCSR0B |= (1 << TXEN0);		// Turn on transmitter
	UCSR0B |= (1 << RXEN0);		// Turn on receiver
	UCSR0C = (3 << UCSZ00);		// Set for async. operation, no parity, 
								// one stop bit, 8 data bits
} 

void string_out(char* input)
{
	unsigned int index = 0;
	while(input[index] != '\0')
	{
		// input[index] = input[index];
		serial_out(input[index]);
		index++;
	}
}

/*
serial_out — Output a byte to the USART0 Port
*/
void serial_out(char ch) {
	while ((UCSR0A & (1 << UDRE0)) == 0 );

	UDR0 = ch; 
}

/*
serial_in - Read a byte from the USART0 and return it
*/

char serial_in() {
	while ( !(UCSR0A & (1 << RXC0)) );
	return UDR0;

}


void timer_init()
{

	    // Set the Timer Mode to CTC
    TCNT1 |= (1 << WGM01);

    // Set the value that you want to count to
    OCR0A = 0x10;

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
	unsigned short baud_rate = 47;
	serial_init(baud_rate);
	timer_init();
	unsigned int timer, old_timer;
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
    flag = 1;//event to be exicuted every 4ms here
}

