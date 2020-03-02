#ifndef PWM_MODULE

/**************************** GLOBAL VARIABLES ***************************/

#define SERVO_PORT PB2							//output pin for servo PWM
#define ADC_THRESHOLD_DIFF ((int) 8)            //ADC values must be at least this far apart
#define PWM_THRESHOLD_HIGH ((int) 1825)         //upper threshold for PWM value (too fast after this value)
#define PWM_THRESHOLD_LOW  ((int) 1818)         //lower threshold for PWM value (too fast after this value)
#define PWM_COUNTER OCR1A                       //increase/decrease to move servo

/**************************** FUNCTIONS ***************************/

void timer_init();
void adc_timer_init();
void servo_init();
void orient_servo(uint8_t left, uint8_t right);

#define PWM_MODULE
#endif