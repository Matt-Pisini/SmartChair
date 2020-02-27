#ifndef ADC_MODULE

/**************************** GLOBAL VARIABLES ***************************/

extern uint8_t ADC_FLAG;
#define LEFT_LDR ADC2
#define RIGHT_LDR ADC3

/**************************** FUNCTIONS ***************************/

void ADC_init();
void ADC_conversion(char LDR);

#define ADC_MODULE
#endif