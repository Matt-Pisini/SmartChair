#include <avr/io.h>
#include "PWM.h"
/********************************************* TIMER CODE FOR PWM ****************************************************************/

void timer_init()
{
    ICR1 = 576;                             //counter counts to this value for 20ms period of PWM

    OCR1A = 43;                             //CONT. MOTION SERVO CONTROL (based off ICR1 = 576):
                                            //No Motion           = 1.5ms HIGH --> OCR1A = 43
                                            //Full anticlockwise  = 1.7ms HIGH --> OCR1A = 37
                                            //Full clockwise      = 1.3ms HIGH --> OCR1A = 43

    TCCR1A |= (1 << COM1A1);                //none-inverted mode (HIGH at bottom, LOW on Match)

    TCCR1A |= (1 << WGM11);                 //sets to fast PWM mode using ICR1 as register to count to
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    TCCR1B |= (1 << CS12);                  //sets prescaler to 256 and starts timer

    sei();                                  //enable global interrupt flag register

}

unsigned int timer_read()
{
  unsigned char sreg;
  unsigned int i;

  sreg = SREG;                              //save global interrupt flag
  cli();                                    //disable interrupts
  i = TIMER;                                //stores counter value

  SREG = sreg;                              //restores previous global interrupt flag
  return i;

}

/********************************************************************************************************************************/


/********************************************* SERVO CODE FOR PWM ****************************************************************/

void init_servo()
{
  DDRB |= (1 << SERVO_PORT);                //sets PB2 to output 
}

/********************************************************************************************************************************/

/********************************************* ISR CODE FOR PWM ****************************************************************/

ISR (TIMER1_COMPA_vect)  // timer1 overflow interrupt service routine
{
  
  timer_flag =  1;
  lcd_stringout("interrupt");

}
/********************************************************************************************************************************/
