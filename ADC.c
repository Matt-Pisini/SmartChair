#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "ADC.h"
#include "LCD.h"


void ADC_init()
{
	ADCSRA |= (1 << ADEN);						//enables ADC module
	ADMUX |= (1 << ADLAR);						//left adjust for 8 bit results
	ADMUX |= (1 << REFS0);						//AVCC for high voltage selection

	ADCSRA |= (1 << ADIE);						//enable ADC interrupt  
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1);		//prescaler = 64
	
	ADCSRA |= (1 << ADATE);						//sets autotrigger (controlled in ADCSRB)
	ADCSRB |= (1 << ADTS2);						//sets timer/counter0 overflow interrupt as trigger
	ADMUX |= 0x02;								//start mux on PC2 (ADC2)

}

void ADC_conversion(char ADC_MUX)
{
	ADMUX &= 0xF0;						//clears MUX[3:0] bits
	ADMUX |= ADC_MUX;					//assigns select bits for correct ADC module
	ADCSRA |= (1 << ADSC);				//initiates one conversion

}


void display_ADC_LCD(char *buf_right, char *buf_left)           //displays ADC values to LCD
{
    if (new_adc_val_right != old_adc_val_right) 
    {
        lcd_writecommand(1);
        lcd_moveto(0, 0);
        lcd_stringout("RIGHT_LDR: ");
        lcd_stringout(buf_right);
        lcd_moveto(1, 0);
        lcd_stringout("LEFT_LDR: ");
        lcd_stringout(buf_left);
        old_adc_val_right = new_adc_val_right;                  //update value for comparison
    }
    if (new_adc_val_left != old_adc_val_left)
    {
        lcd_writecommand(1);
        lcd_moveto(0, 0);
        lcd_stringout("RIGHT_LDR: ");
        lcd_stringout(buf_right);
        lcd_moveto(1, 0);
        lcd_stringout("LEFT_LDR: ");
        lcd_stringout(buf_left);
        old_adc_val_left = new_adc_val_left;                    //update value for comparison
    }
}


void update_ADC(char *buf_right, char *buf_left)                //updates ADC value when ready
{
    if ((ADMUX & 0x0F) == RIGHT_LDR)                            //check if RIGHT_LDR mux value
    {
        new_adc_val_right = ADC_VALUE;
        ADMUX &= 0xF0;                                          //clears MUX bits for ADC        
        ADMUX |= LEFT_LDR;                                      //assigns select bits for next ADC measurement (LEFT_LDR)
        buf_right[0] = '\0';                                    //clear buffers
        sprintf(buf_right, "%u", new_adc_val_right);        //convert byte into string of int value
    }
    else
    {
        new_adc_val_left = ADC_VALUE;
        ADMUX &= 0xF0;                                          //clears MUX bits for ADC
        ADMUX |= RIGHT_LDR;                                     //assigns select bits for next ADC measurement (RIGHT_LDR)  
        buf_left[0] = '\0';                                     //clear buffers
        sprintf(buf_left, "%u", new_adc_val_left);          //convert byte into string of int value
    }
}


//interrupt service routine
ISR (ADC_vect)
{

	ADC_COMPLETE_FLAG = 1;						//set ADC flag when conversion complete

}