#ifndef SERIAL_CONTROL

/**************************** FUNCTIONS ***************************/

void serial_out_string(char* output);
void serial_init(unsigned short ubrr);
void serial_out_byte(char ch);
char serial_in();

#define SERIAL_CONTROL
#endif