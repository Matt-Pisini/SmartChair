#ifndef PWM_MODULE

/**************************** GLOBAL VARIABLES ***************************/

#define SERVO_PORT PB2
#define TIMER TCNT1

/**************************** FUNCTIONS ***************************/

void timer_init();
void init_servo();

#define PWM_MODULE
#endif