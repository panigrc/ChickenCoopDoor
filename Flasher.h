#include "Arduino.h"

// Flasher class forked from Adafruit (https://learn.adafruit.com/multi-tasking-the-arduino-part-1/all-together-now)

class Flasher
{
  // Class Member Variables
  // These are initialized at startup
  int ledPin;      // the number of the LED pin
  long OnTime;     // milliseconds of on-time
  long OffTime;    // milliseconds of off-time
  int Times;       // how many times to flash

  // These maintain the current state
  int ledState;             		// ledState used to set the LED
  unsigned long previousMillis;  	// will store last time LED was updated

  // Constructor - creates a Flasher
  // and initializes the member variables and state

public:
  Flasher(int pin)
  {
    ledPin = pin;
    pinMode(ledPin, OUTPUT);

    OnTime = 0;
    OffTime = 0;
    Times = 0;

    ledState = LOW;
    previousMillis = 0;
  }

  void On()
  {
    ledState = HIGH;
    digitalWrite(ledPin, ledState);
  }

  void Off()
  {
    ledState = LOW;
    digitalWrite(ledPin, ledState);
  }

  void Flash(int times, long on, long off)
  {
    Times = times;
    OnTime = on;
    OffTime = off;
  }

  void Update()
  {
    // check to see if it's time to change the state of the LED
    unsigned long currentMillis = millis();

    if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
    {
      previousMillis = currentMillis;  // Remember the time
      Off();
    }
    else if (Times > 0 && (ledState == LOW) && (currentMillis - previousMillis >= OffTime))
    {
      Times--;
      previousMillis = currentMillis;   // Remember the time
      On();
    }
  }
};
