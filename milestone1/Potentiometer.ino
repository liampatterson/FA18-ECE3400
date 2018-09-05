/*
  Potentiometer and Servos

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#include <Servo.h>

int LEDOut= 3;
int potentiometerIn = A1;
int readVal= 0;
Servo servoOne;
int temp= 0;
// the setup function runs once when you press reset or power the board
void setup() {
  servoOne.attach(5);
  pinMode(LEDOut, OUTPUT);
  pinMode(potentiometerIn, INPUT);
  Serial.begin(9600); //set baud rate
 
}

// the loop function runs over and over again forever
void loop() {
  readVal= analogRead(potentiometerIn);  //read the potentiometer
  Serial.println(readVal);
  //analogWrite(LEDOut, (readVal/4));
  servoOne.write(readVal); //writing 0 is full speed clockwise, 90 is stopped, 180 is full speed counterclockwise
  //delay(50);                       // wait for a half second
}
