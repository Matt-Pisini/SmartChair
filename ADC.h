#ifndef ADC_MODULE

/**************************** GLOBAL VARIABLES ***************************/

extern volatile uint8_t ADC_FLAG;

/**************************** FUNCTIONS ***************************/

void ADC_init();
void ADC_conversion(char ADC_MUX);

#define ADC_MODULE
#endif