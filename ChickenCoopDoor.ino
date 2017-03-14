/* a simple chicken coop door controller
   chicken door has 2 terminal switches and 1 dc motor
   also the time is taken from an external time keeper
*/


#include <Wire.h>
#include <DS1307RTC.h>
#include "Flasher.h"
//#include "Buzzer.h"
#include <Time.h>
#include <Timezone.h>


boolean rtcError = false;

// LED
Flasher statusLED(5); // shows the status

// *** Chicken Door ***

// Opening Closing Times (12x for each month) in local time
// in minutes. For example 19:00 would be 19*60 = 1140
int doorOpeningTime[12] = {
                            470, // Jan
                            444, // Feb
                            400, // Mar
                            411, // Apr
                            376, // May
                            362, // Jun
                            375, // Jul
                            403, // Aug
                            432, // Sep
                            461, // Oct
                            435, // Nov
                            465, // Dec
                           };

int doorClosingTime[12] = {
                            1140,
                            1140,
                            1200,
                            1230,
                            1260,
                            1260,
                            1260,
                            1260,
                            1230,
                            1200,
                            1170,
                            1140,
                          };


// Motor pins - going to the H bridge L293D
const byte motorEnablePin = 8;
const byte motorIn1Pin = 7;
const byte motorIn2Pin = 6;

// Motor state , if motor is running or not
boolean motorState = false;

// Terminal Swtiches (buttons)
const byte buttonTop = A3;
byte buttonStateTop	= 0;
const byte buttonBottom = A2;
byte buttonStateBottom = 0;

// Alarm system

//Buzzer alarmBuzzer(3); // PWM
//const byte buttonAlarm = 4;
//byte buttonAlarmState = 0;

// Timezone

TimeChangeRule grDST = {"EEST", Last, Sun, Mar, 3, 180};    //Daylight time (summer)
TimeChangeRule grSTD = {"EET", Last, Sun, Oct, 4, 120};     //Standard time
Timezone grTZ(grDST, grSTD);

time_t local_time;

unsigned long previousDisplayTimeMillis = 0;

void setup()
{
  // RTC
  Wire.begin();
  Serial.begin(115200);

  if (RTC.get())
  {
    rtcError = false;
    
    setSyncProvider(RTC.get); // the function to get the time from the RTC
    setSyncInterval(3600); // re-sync every seconds
    local_time = grTZ.toLocal(now()); // get the local time
    statusLED.Flash(hour(local_time), 500, 500);
  }
  else
  {
    rtcError = true;

    statusLED.Flash(3, 300, 300);
  }

  // *** Chicken Door ***

  // Motor setup
  pinMode(motorIn1Pin, OUTPUT);
  pinMode(motorIn2Pin, OUTPUT);
  pinMode(motorEnablePin, OUTPUT);

  // Button setup
  pinMode(buttonTop, INPUT);
  pinMode(buttonBottom, INPUT);

  // Alarm setup
  //pinMode(buttonAlarm, INPUT);

}// END setup


void loop()
{

  // if both buttons are pressed test the door
  testDoor();

  //alarm();

  if (rtcError == false) // if rtc is working
  {
    // if time is set, doorLoop 
    if (timeStatus() == timeSet)
    {
      local_time = grTZ.toLocal(now()); // get the local time
      if (millis() > previousDisplayTimeMillis + 2000) // run every 2 seconds
      {
        serialDisplayTime();
        serialAdjustTime(now());
        previousDisplayTimeMillis = millis();
      }
      doorLoop();
    }
    else
    {
      serialSetTime();
    }  
  }

  statusLED.Update();
  //alarmBuzzer.Update();

}// END loop


// Tells us if it's time to close the door
// If 1 the door should open, 0 should close
// parameters 2 int arrays
boolean door_open(int openingTime[], int closingTime[])
{

  // if it's between opening and closing time then door should open
  if ( hour(local_time) * 60 + minute(local_time) > openingTime[month(local_time) - 1] && hour(local_time) * 60 + minute(local_time) < closingTime[month(local_time) - 1] )
  {
    return true;
  }
  else
  {
    return false;
  }

}// END door_open


// Door main function, is called by the Loop
// Checks terminal buttons, and opens or closes the door
void doorLoop()
{
  motorState = false;
  buttonStateTop			= digitalRead(buttonTop);
  buttonStateBottom		= digitalRead(buttonBottom);

  // if the door is in the middle
  if (buttonStateTop == LOW && buttonStateBottom == LOW)
  {
    // open
    if (door_open(doorOpeningTime, doorClosingTime))
    {
      motorState = setDoorMotor(255, 1);
    }
    // close
    else
    {
      motorState = setDoorMotor(255, 0);
    }
  }
  // it's time to open and the door is in the Bottom edge
  if (buttonStateBottom == HIGH && door_open(doorOpeningTime, doorClosingTime))
  {
    motorState = setDoorMotor(255, 1);
  }
  // it's time to close and the door is in the Top edge
  if (buttonStateTop == HIGH && !door_open(doorOpeningTime, doorClosingTime))
  {
    motorState = setDoorMotor(255, 0);
  }
  // if motorState = 0, no criteria was met, so disable the motor
  if (motorState == false)
  {
    setDoorMotor(0, 0);
    //digitalWrite(statusLED, LOW);
  }
  else
  {
    //digitalWrite(statusLED, HIGH);
  }
  
}// END doorLoop


/*
   speed = enable pin of the L293 just turns the motor on or off
   the pins in1 and in2 must be set to opposite values for the direction
   If in1 is HIGH and in2 is LOW, the motor will spin one way,
   if on the other hand in1 is HIGH and in2 LOW then the motor will spin
   in the opposite direction.
   The '!' command means 'not'. So the first digitalWrite command for in1 sets
   it to the opposite of whatever the value of 'reverse' is, so if reverse is
   HIGH it sets it to LOW and vice versa.
   The second digitalWrite for 'in2' sets the pin to whatever the value of
   'reverse' is. This means that it will always be the opposite of whatever in1 is.
*/
boolean setDoorMotor(int speed, boolean reverse)
{

  analogWrite(motorEnablePin, speed);
  digitalWrite(motorIn1Pin, ! reverse);
  digitalWrite(motorIn2Pin, reverse);
  return true;

} // END setDoorMotor


// function to test the door
// happens only when both terminal buttons are pressed
void testDoor()
{
  
  buttonStateTop = digitalRead(buttonTop);
  buttonStateBottom = digitalRead(buttonBottom);

  if (buttonStateTop == HIGH && buttonStateBottom == HIGH)
  {
    boolean reachedTop = false;
    boolean reachedBottom = false;
    statusLED.On();
    delay(1000);

    while (reachedTop == false || reachedBottom == false)
    {
      motorState = false;
      buttonStateTop = digitalRead(buttonTop);
      buttonStateBottom = digitalRead(buttonBottom);

      // if the door is in the middle
      if (buttonStateTop == LOW && buttonStateBottom == LOW)
      {
        // open
        if (reachedBottom == true)
        {
          motorState = setDoorMotor(255, 1);
        }
        // close
        else if (reachedTop == true)
        {
          motorState = setDoorMotor(255, 0);
        }
      }
      // it's time to open
      if (buttonStateBottom == HIGH && buttonStateTop == LOW)
      {
        motorState = setDoorMotor(255, 1);
        reachedBottom = true;
      }
      // it's time to close
      if (buttonStateTop == HIGH && buttonStateBottom == LOW)
      {
        motorState = setDoorMotor(255, 0);
        reachedTop = true;
      }
    }
    setDoorMotor(0, 0);



    statusLED.Off();
    //statusLED.Flash(now.hour(), 500, 500);

    //delay(3000);
  }
  
} // END testDoor


// proccesses serial message to set time
time_t serialSetTime()
{

  // if time sync available from serial port, update time and return true
  if (Serial.available() > 0)   // time message consists of header & ASCII digits
  {
    delay(10);

    char c = Serial.read() ;
    Serial.print(c);

    if ( c == 'T' )
    {
      time_t pctime = 0;
      while (Serial.available() > 0)
      {
        c = Serial.read();
        if ( c >= '0' && c <= '9')
        {
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number
        }
      }
      RTC.set(pctime);
      setTime(pctime);
      return pctime;   // Sync Arduino clock to the time received on the serial port
    }
  }

} //END processSerialMessage

// proccesses serial message to adjust time
time_t serialAdjustTime(time_t pctime)
{

  // if time sync available from serial port, update time and return true
  if (Serial.available() > 0)   // time message consists of header & ASCII digits
  {
    delay(10);

    char c = toupper(Serial.read());
    


    if ( c == 'A' )
    {
          Serial.println("Type number of milis to adjust time + or -");
      int time_adjust = 0;
        while ( time_adjust == 0)
  {
     if (Serial.available() > 0) time_adjust = Serial.parseInt();
  }
      Serial.println(time_adjust);
  
      pctime += time_adjust;
      
      RTC.set(pctime + time_adjust);
      setTime(pctime + time_adjust);
      return pctime;   // Sync Arduino clock to the time received on the serial port
    }
  }

} //END processSerialMessage

// prints time in Serial
void serialDisplayTime()
{
  
  Serial.print("UTC time: ");
  Serial.print(timeToString(now()));
  Serial.print("  Local time: ");
  Serial.println(timeToString(local_time));

}// END serialDisplayTime


// format timestamp to string
char * timeToString(time_t t)
{
  static char str[17];

  sprintf(str, "%02d:%02d %02d/%02d/%d", hour(t), minute(t), day(t), month(t), year(t));

  return str;
  
} //END timeToString


/*
  void alarm()
  {
	buttonAlarmState = digitalRead(buttonAlarm);
	if (buttonAlarmState == HIGH && now.hour() > 0 && now.hour() < 7 && alarmBuzzer.Queue() == 0)
	{
		alarmBuzzer.Buzz(100, 100, 100, 255, 130);
	}
  }
*/
