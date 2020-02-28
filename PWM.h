#ifndef PWM_MODULE

/**************************** GLOBAL VARIABLES ***************************/

#define SERVO_PORT PB2
#define TIMER TCNT1
extern volatile char ADC_LEFT_RIGHT_FLAG;

/**************************** FUNCTIONS ***************************/

void timer_init();
void adc_timer_init();
void init_servo();

#define PWM_MODULE
#endif