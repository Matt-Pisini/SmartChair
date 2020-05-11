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

void serial_init(unsigned short MYUBRR) {
    _delay_ms(300);        // Delay 3s for Emic 2 to initialize
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


int main(void)
{
    serial_init()
    unsigned short MYUBBR = BAUD;
    char input;
    while(1)
    {
    switch (input) {
        case 0:
            emic_stringout(str1)
            break;
        case 1:
            emic_stringout(str2)
            break;
        case 2:
            emic_stringout(str3)
            break;
        case 3:
            emic_stringout(str4)
            break;
        case 4:
            emic_stringoutg(str5)
            break;
        case 5:
            emic_stringout(str6)
            break;
        case 6:
            emic_stringout(str7)
            break;
        case 7:
            emic_stringout(str8)
            break;
        case 8:
            emic_stringout(str9)
            break;
        case 9:
            emic_stringout(str10)
            break;
        case 10:
            emic_stringout(str11)
            break;
            
        default:
            emic_stringout(str12)
        }
    
        return 0;
    }



