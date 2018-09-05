#include <Servo.h>

// light sensor port numbers
int lightLeftPort = A0;
int lightRightPort = A1;

// light sensor read values
int lightLeftVal = 0;
int lightRightVal = 0;

// servo objects
Servo servoLeft;
Servo servoRight;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  servoSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  readLightSensors();
}

void servoSetup() {
  servoLeft.attach( 5 );
  servoRight.attach( 6 );
}

void goForward() {
  servoLeft.write
}

void readLightSensors() {
    lightLeftVal = analogRead( lightLeftPort );
    lightRightVal = analogRead( lightRightPort );
    Serial.println( "Left: " + lightLeftVal + ", Right: " + lightRightVal );
}
