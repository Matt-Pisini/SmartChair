#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600              // Baud rate used by the Emic 2
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register

char str1[] = "Welcome";
char str2[] = "Main Menu";
char str3[] = "UV Rating";
char str4[] = "Tanning Session";
char str5[] = "Set Timer";
char str6[] = "Adjust Timer";
char str7[] = "Run Timer";
char str8[] = "Please Flip";
char str9[] = "Tanning Done";
char str10[] = "Apply Sunscreen";
char str11[] = "Thank you for using smart chair";
char star12[] = "Something Wrong";

void serial_init(void) {
    _delay_ms(300);        // Delay 3s for Emic 2 to initialize
    UBRR0 = MYUBRR;          // Set baud rate
    UCSR0B |= (1 << TXEN0);  // Turn on transmitter
    UCSR0B |= (1 << RXEN0);  // Turn on receiver to check for functionality
    UCSR0C = (3 << UCSZ00);  // Set for asynchronous operation, no parity,
                             // one stop bit, 8 data bits
}

//Read a byte from the USART0 and return it
char serial_in() {
    while ( !(UCSR0A & (1 << RXC0)) );
    return UDR0;

}

//Output a byte to the USART0 Port, test purpose
void serial_out(char ch) {
    while ((UCSR0A & (1 << UDRE0)) == 0 );
    UDR0 = ch;
}

//Print the contents of the character string out the serial port.
void serial_out_string(char *str)
{
    char ch;
    while ((ch = *str++) != '\0')
        serial_out_string(ch);
}

int main(void)
{
    char input;
    input = serial_in();
    while(1)
    {
    switch (input) {
        case 0:
            serial_out_string(str1)
            break;
        case 1:
            serial_out_string(str2)
            break;
        case 2:
            serial_out_string(str3)
            break;
        case 3:
            serial_out_string(str4)
            break;
        case 4:
            serial_out_string(str5)
            break;
        case 5:
            serial_out_string(str6)
            break;
        case 6:
            serial_out_string(str7)
            break;
        case 7:
            serial_out_string(str8)
            break;
        case 8:
            serial_out_string(str9)
            break;
        case 9:
            serial_out_string(str10)
            break;
        case 10:
            serial_out_string(str11)
            break;
            
        default:
            serial_out_string(str12)
        }
    
        return 0;
    }


