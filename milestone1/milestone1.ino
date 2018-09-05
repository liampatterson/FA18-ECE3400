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
  //servoSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  readLightSensors();
  delay( 200 );
}

void servoSetup() {
  servoLeft.attach( 5 );
  servoRight.attach( 6 );
}

void goForward() {
  servoLeft.write( 180 );
  servoRight.write( 0 );
}

void goSlightRight() {
  servoLeft.write( 180 );
  servoRight.write( 120 );
}

void goRight() {
  servoLeft.write( 180 );
  servoRight.write( 90 );
}

void goSlightLeft() {
  servoLeft.write( 60 );
  servoRight.write( 0 );
}

void readLightSensors() {
    lightLeftVal = readQD( lightLeftPort );
    lightRightVal = analogRead( lightRightPort );
    String left = "Left: ";
    String right = ", Right: ";
    String lightOutput = left + lightLeftVal + right + lightRightVal;
    Serial.println( lightOutput );
}

int readQD( int QRE1113_Pin ) {
  //Returns value from the QRE1113
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  pinMode( QRE1113_Pin, OUTPUT );
  digitalWrite( QRE1113_Pin, HIGH );
  delayMicroseconds(10);
  pinMode( QRE1113_Pin, INPUT );

  long time = micros();

  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while ( digitalRead( QRE1113_Pin ) == HIGH && micros() – time < 3000 ); int diff = micros() - time; return diff;
  }
