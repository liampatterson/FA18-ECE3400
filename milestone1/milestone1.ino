#include <Servo.h>

int lightLeft = A0;
int lightRight = A1;

int valLeft = 0;
int valRight = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  readLightSensors();
}

void servoSetup() {
  
}

void readLightSensors() {
    valLeft = analogRead( lightLeft );
    valRight = analogRead( lightRight );
    Serial.println( "Left: " + valLeft + ", Right: " + valRight );
}
