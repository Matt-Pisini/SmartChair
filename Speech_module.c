//:SHello! My name is Emic 2. Nice to meet you. 
//<audio output>
//：

#include <SoftwareSerial.h>
 
#define rxPin 2 //serial input (connects to Emic Sout)
#define txPin 3 //serial output (connects to Emic Sin)

// set up a new serial port
SoftwareSerial emicSerial =  SoftwareSerial(rxPin, txPin);

void setup()  // Set up code once start up
{
 	// define pin modes
 	pinMode(rxPin, INPUT);
 	pinMode(txPin, OUTPUT);
  
 	// set rate for the SoftwareSerial port
 	emicSerial.begin(9600);


emicSerial.print('\n');             // Send a CR in case the system is already up
while (emicSerial.read() != ':');   // When the Emic 2 has initialized and is ready, it will send a single ':' character, so wait here until we receive it
delay(10);                          // Short delay
emicSerial.flush();                 // Flush the receive buffer
}

void loop()  // Main code
{
	// Speak some text
	emicSerial.print('S');
	emicSerial.print("Hello. My name is the Smart Chair");  // Send the desired string to convert to speech
	emicSerial.print('\n');
	while (emicSerial.read() != ':');   // Wait here until the Emic 2 responds with a ":" indicating it's ready to accept the next command
	delay(500);    // half second delay
    
    char Voice; //create a character so the emic module will accept it

    switch (Voice) {
   
  	case 0: 
   		emicSerial.print("Welcome");
   		break;   
  	case 1: 
   		emicSerial.print("Main Menu");   
   		break;
  	case 2: 
   		emicSerial.print("UV Rating");   
   		break;   
  	case 3: 
   		emicSerial.print("Tanning Session");   
   		break;
 	case 4: 
   		emicSerial.print("Set Timer");   
   		break;   
  	case 5: 
   		emicSerial.print("Adjust Timer");   
   		break;
  	case 6: 
   		emicSerial.print("Run Timer");   
   		break;
  	case 7: 
   		emicSerial.print("Please Flip");   
   		break;   
  	case 8: 
   		emicSerial.print("Tanning Done");   
   		break;
  	case 9: 
   		emicSerial.print("Apply Sunscreen");   
   		break;   
  	case 10: 
   		emicSerial.print("Thank you for using smart chair");   
   		break;
  
	default:
   		emicSerial.print("Wrong Message");
   	}

}






