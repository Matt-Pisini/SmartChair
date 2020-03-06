#ifndef LCD_STRING_STATES

/**************** LCD STRING STATE MACHINE (FLASH MEM. 32KB) ****************/

/*
	NOTE: each string can be AT MOST 19 characters. The LCD screen is only
	20 characters and we have chosen to start on column 1 because of the 
	cursor display. If it is more than this it will mess up display...
*/

                    /******STATE 0: Splash Screen ******/

    const char state0_string0[] PROGMEM = "Welcome to";
    const char state0_string1[] PROGMEM = "SmartChair";
    const char state0_string2[] PROGMEM = "------------------";
    const char state0_string3[] PROGMEM = "------------------";

    				/******STATE 1: ******/

    const char state1_string0[] PROGMEM = "STUFF3";
    const char state1_string1[] PROGMEM = "STUFF2";
    const char state1_string2[] PROGMEM = "STUFF1";

        			/******STATE 2: ******/


#define LCD_STRING_STATES
#endif