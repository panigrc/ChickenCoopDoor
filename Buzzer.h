#include "Arduino.h"

// Buzzer class forked from Adafruit (https://learn.adafruit.com/multi-tasking-the-arduino-part-1/all-together-now)

class Buzzer
{
	// Class Member Variables
	// These are initialized at startup
	int buzzerPin;      // the number of the Buzzer pin
	long OnTime;     // milliseconds of on-time
	long OffTime;    // milliseconds of off-time
	int Times;       // how many times to buzz

	// These maintain the current state
	int buzzerState;             		// current buzzerState
	int buzzerStateOn;             		// buzzerStateOn used to set the Buzzer
	int buzzerStateOff;             		// buzzerStateOff used to set the Buzzer
	unsigned long previousMillis;  	// will store last time Buzzer was updated

	// Constructor - creates a Buzzer
	// and initializes the member variables and state
	
	public:
	Buzzer(int pin)
	{
		buzzerPin = pin;
		pinMode(buzzerPin, OUTPUT);
		
		OnTime = 0;
		OffTime = 0;
		Times = 0;

		buzzerState = 0; // current state to 0
		buzzerStateOn = 170; //default On State
		buzzerStateOff = 0;
		previousMillis = 0;
	}
	
	void Buzz(int times, long on, long off, int stateOn, int stateOff)
	{
		Times = times;
		OnTime = on;
		OffTime = off;
		buzzerState = stateOn;
		buzzerStateOn = stateOn;
		buzzerStateOff = stateOff;
	}
	
	void Update()
	{
		// check to see if it's time to change the state of the LED
		unsigned long currentMillis = millis();

		if (buzzerState == buzzerStateOn && currentMillis - previousMillis >= OnTime)
		{
			previousMillis = currentMillis;  // Remember the time
			buzzerState = buzzerStateOff;
			analogWrite(buzzerPin, buzzerStateOff); // turn off
		}
		else if (Times > 0 && buzzerState == buzzerStateOff && (currentMillis - previousMillis >= OffTime))
		{
			Times--;
			previousMillis = currentMillis;   // Remember the time
			buzzerState = buzzerStateOn;
			analogWrite(buzzerPin, buzzerStateOn);
		}
		else if (Times == 0)
		{
			buzzerState = 0;
			analogWrite(buzzerPin, 0);
		}
	}
	
	int Queue()
	{
		return Times;
	}
};

