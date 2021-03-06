#ifndef LCD_STRING_STATES

/**************** LCD STRING STATE MACHINE (FLASH MEM. 32KB) ****************/

/*
	NOTE: each string can be AT MOST 19 characters. The LCD screen is only
	20 characters and we have chosen to start on column 1 because of the 
	cursor display. If it is more than this it will mess up display...
*/

                    /******STATE 0: Splash Screen ******/

    const char state0_string1[] PROGMEM = "Welcome to";
    const char state0_string2[] PROGMEM = "SmartChair";
    const char state0_string3[] PROGMEM = "------------------";
    const char state0_string4[] PROGMEM = "------------------";
    const int8_t state0_transitions[] = {-1,-1,-1,-1}; 

    				/******STATE 1: ******/

    const char state1_string1[] PROGMEM = "Tan";
    const char state1_string2[] PROGMEM = "Check UV";
    const int8_t state1_transitions[] = {3,2}; 

        			/******STATE 2: ******/

    const char state2_string1[] PROGMEM = "UV Reading: xx";
    const char state2_string2[] PROGMEM = "The UV is: ";			//fill in low, moderate, or high. Need UV_RATING variable for this
    const char state2_string3[] PROGMEM = "";
    const char state2_string4[] PROGMEM = "Back";
    const int8_t state2_transitions[] ={-1, -1, -1, 1};
        			/******STATE 3: ******/

    const char state3_string1[] PROGMEM = "Select Skin Type:";
    const char state3_string2[] PROGMEM = "Fair";
    const char state3_string3[] PROGMEM = "Medium";
    const char state3_string4[] PROGMEM = "Dark";
    const char state3_string5[] PROGMEM = "Back";
    const int8_t state3_transitions[] ={-1, 4, 4, 4, 1};


        			/******STATE 4: ******/

    const char state4_string1[] PROGMEM = "We recommend xx";			//need function x(UV, skin) = time
    const char state4_string2[] PROGMEM = "minutes in the sun.";
    const char state4_string3[] PROGMEM = "Tan for xx minutes?";
    const char state4_string4[] PROGMEM = "Different Time";
    const int8_t state4_transitions[] ={-1,-1,6,5};

        			/******STATE 5: ******/

    const char state5_string1[] PROGMEM = "Tan for: xx min";
    const char state5_string2[] PROGMEM = "";
    const char state5_string3[] PROGMEM = "Continue";
    const char state5_string4[] PROGMEM = "Back";
    const int8_t state5_transitions[] ={-1,-1,6,4};

            		/******STATE 6: ******/

    const char state6_string1[] PROGMEM = "Time left: xx min.";
    const char state6_string2[] PROGMEM = "Flip sides in xx min.";
    const char state6_string3[] PROGMEM = "";
    const char state6_string4[] PROGMEM = "End session";
    const int8_t state6_transitions[] ={-1,-1,-1,1};

        			/******STATE 7: ******/

    const char state7_string1[] PROGMEM = "Please flip!";
    const char state7_string2[] PROGMEM = "Time will resume.";
    const int8_t state7_transitions[] ={-1,-1};

            		/******STATE 8: ******/

    const char state8_string1[] PROGMEM = "Your time in the";
    const char state8_string2[] PROGMEM = "sun is up!";
    const char state8_string3[] PROGMEM = "Seek shade";
    const char state8_string4[] PROGMEM = "Add time";
    const int8_t state8_transitions[] ={-1,-1,10,9};

            		/******STATE 9: ******/

    const char state9_string1[] PROGMEM = "Warning: to";
    const char state9_string2[] PROGMEM = "continue, apply ";
    const char state9_string3[] PROGMEM = "SPF 30+ sunscreen.";
    const char state9_string4[] PROGMEM = "Add time";
    const int8_t state9_transitions[] ={-1,-1,-1,5};

             		/******STATE 10: ******/

    const char state10_string1[] PROGMEM = "Thank you for using";
    const char state10_string2[] PROGMEM = "SmartChair! Relax";
    const char state10_string3[] PROGMEM = "and get some shade";
    const char state10_string4[] PROGMEM = "Main Menu";
    const int8_t state10_transitions[] ={-1,-1,-1,1};
   

#define LCD_STRING_STATES
#endif