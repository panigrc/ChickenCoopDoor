# ChickenCoopDoor

ChickenCoopDoor is an Arduino project which aims to the automation of chicken coop doors. By having a Real Time Clock we can set specific door opening and closing time for each month.

### Features

  - Utilization of the millis function in order to avoid delays
  - External Status LED
  - Test Procedure, to test the complete functionality of the door
  - Read, set and adjust the time from the Serial with your PC
  - Timezone usage, in order to adjust the time according to the daylight savings time

### Changelog
  - 1.0 Initial Commit

### External Libraries Used
All the other libraries can be found and downloaded from the library management of the Arduino IDE.

* [Arduino Timezone Library v1.0](https://github.com/JChristensen/Timezone)

### Hardware Used

* Arduino Nano
* L298N Dual H Bridge DC Stepper Motor Drive Controller Board Module for Arduino
* Real Time Clock Module For AVR ARM PIC New I2C RTC DS1307 AT24C32
* 2x LEDs
* 2x Tact Switch KW11-3Z 5A 250V Microswitch 3PIN Buckle Hot (Limit Switches)

### Test Procedure
By tapping both limit switches simultaneusly the door will get in the Test "phase". The 2nd LED will start blinking so many times as the current hour and the door will do a complete opening or closing cycle.

### Setting the Time
By connecting to the Serial and Pressing 'T' and then the UTC timestamp you can set the time of the RTC.

By pressing 'A' and then + or - millis you can adjust the time accordingly.

### Demo

[You can watch a demo video here](https://www.youtube.com/watch?v=tN1-I40H8xY)

### Future Updates

I would like to update the Real Time Clock to use the DS3231 which is more accurate and has more precission than the DS1307 which has a significant time deviation after some days. The DS3231 has an internal thermistor which adjusts the time according to the temperature, which the DS1307 hasn't.

Also the Alarm feature is commented out because I didn't use it. The theory was to add an extra limit switch on the door and a buzzer so it would have an alarm functionality whenever the door opens in the chicken coop door closing time (at night basically).

### License

GPL v 3.0