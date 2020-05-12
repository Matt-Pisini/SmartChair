#include <avr/io.h>
#include <util/delay.h>
#include "speech_module.h"

void serial_init(unsigned short MYUBRR) {
    UBRR0 = MYUBRR;          // Set baud rate
    UCSR0B |= (1 << TXEN0);  // Turn on transmitter
    UCSR0C = (3 << UCSZ00);  // Set for asynchronous operation, no parity,
                             // one stop bit, 8 data bits
}

//Output a byte to the USART0 Port, test purpose
void serial_out(char ch) {
    while ((UCSR0A & (1 << UDRE0)) == 0 );
    UDR0 = ch;
}

//Print the contents of the character string out the serial port.
void serial_out_string(char *s)
{
    char ch;
    while ((ch = *s) != '\0')
        serial_out(ch);
}

void emic_stringout(char *str)
{
    serial_out_string(str);              // Output the string
}

