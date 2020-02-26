/*
serial_init - Initialize the USART port
*/
#include <avr/io.h>
#include "serial.h"

void serial_init(unsigned short ubrr) {

	UBRR0 = ubrr;				// Set baud rate
	UCSR0B |= (1 << TXEN0);		// Turn on transmitter
	UCSR0B |= (1 << RXEN0);		// Turn on receiver
	UCSR0C = (3 << UCSZ00);		// Set for async. operation, no parity, 
								// one stop bit, 8 data bits
} 
/*
serial_out — Output a byte to the USART0 Port
*/

void serial_out_byte(char ch) {
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

void serial_out_string(char* output)
{
	int index = 0;
	while(output[index] != '\0')
	{
		serial_out_byte(output[index]);
		index++;
	}
}
