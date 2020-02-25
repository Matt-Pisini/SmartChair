#ifndef PWM_MODULE
#define PWM_MODULE

/**************************** GLOBAL VARIABLES ***************************/

#define SERVO_PORT PB2
#define TIMER TCNT1
volatile uint8_t timer_flag;

/**************************** FUNCTIONS ***************************/

void timer_init();
unsigned int timer_read();
void init_servo();

#endif