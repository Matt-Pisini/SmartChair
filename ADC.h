#ifndef ADC_MODULE

/**************************** GLOBAL VARIABLES ***************************/
#define ADC_VALUE ADCH                          //register where 8-bit ADC value kept
extern volatile uint8_t ADC_COMPLETE_FLAG; 
extern volatile uint8_t new_adc_val_left, new_adc_val_right;
extern volatile uint8_t old_adc_val_left, old_adc_val_right;
static const char LEFT_LDR = 0x02;              //left LDR pin on board. Corresponds to PC2 (ADC2)
static const char RIGHT_LDR = 0x03;             //right LDR pin on board. Corresponds to PC3(ADC3)

/**************************** FUNCTIONS ***************************/

void ADC_init();
void adc_timer_init();
void ADC_conversion(char ADC_MUX);
void display_ADC_LCD(char *buf_right, char *buf_left);
void update_ADC(char *buf_right, char *buf_left);


#define ADC_MODULE
#endif