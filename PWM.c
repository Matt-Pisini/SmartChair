#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM.h"

/********************************************* TIMER CODE FOR PWM ****************************************************************/

void timer_init()
{
    TCNT1 = 0;                              //initialize counter value to 0
    OCR1A = 42;                             //CONT. MOTION SERVO CONTROL (based off ICR1 = 576):
                                            //No Motion           = 1.5ms HIGH --> OCR1A = 42
                                            //Full anticlockwise  = 1.3ms HIGH --> OCR1A = 37
                                            //Full clockwise      = 1.7ms HIGH --> OCR1A = 48
    ICR1 = 575 + OCR1A;                     //575 = 20ms, counter counts for 20ms after reaching PWM count


    TIMSK1 |= (1 << TOIE1) | (1 << OCIE1A); //enable overflow interrupt & CompareA interrupt

    TCCR1A |= (1 << WGM11);                 //sets to fast PWM mode using ICR1 as register to count to
    TCCR1B |= (1 << WGM12) | (1 << WGM13);

    TCCR1B |= (1 << CS12);                  //sets prescaler to 256 and starts timer
    PORTB |= (1 << SERVO_PORT);
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
  
  PORTB ^= (1 << SERVO_PORT);

}

ISR (TIMER1_OVF_vect)  // timer1 overflow interrupt service routine
{

  PORTB ^= (1 << SERVO_PORT);

}

/********************************************************************************************************************************/
