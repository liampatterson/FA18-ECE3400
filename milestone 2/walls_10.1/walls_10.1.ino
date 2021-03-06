#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
#include <Servo.h>

// light sensor port numbers
int lightLeftPort = A0;
int lightMiddlePort = A1;
int lightRightPort = A2;

int avgRightDistance = 0;
int avgMiddleDistance = 0;
int avgLeftDistance = 0;

// light sensor read values
int lightMiddleVal = 0;
int lightRightVal = 0;
int lightLeftVal = 0;

//MIDDLE
int upperMiddleBoundWhite = 600;
int lowerMiddleBoundWhite = 0;

int upperMiddleBoundBlack = 900;
int lowerMiddleBoundBlack = 600;

//RIGHT
int upperRightBoundWhite = 450;
int lowerRightBoundWhite = 0;

int upperRightBoundBlack = 900;
int lowerRightBoundBlack = 450;

////LEFT
int upperLeftBoundWhite = 450;
int lowerLeftBoundWhite = 0 ;

int upperLeftBoundBlack = 900;
int lowerLeftBoundBlack = 450;

// a vertex for the left
//int upperLeftBoundVertexWhite = 65;
//int lowerLeftBoundVertexWhite = 0;

boolean middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
boolean rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
boolean rightIsWhite = ( lightRightVal < upperRightBoundWhite );
boolean leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
boolean leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
boolean middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );

boolean foundVertex = ( leftIsWhite && middleIsWhite && rightIsWhite );

int LeftDistance = 0;
int MiddleDistance = 0;
int RightDistance = 0;

int countLeftTurns = 0;
int countRightTurns = 0;

int turns = 0;

// servo objects
Servo servoLeft;
Servo servoRight;


void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  ledSetup();
  servoSetup();
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}

void servoSetup() {
  servoLeft.attach( 9 );
  servoRight.attach( 10 );
}

void ledSetup() {
  pinMode( 0, OUTPUT ); //green LED
  pinMode( 1, OUTPUT ); //white LED
  pinMode( 2, OUTPUT ); //red LED
  pinMode( 4, OUTPUT ); //blue LED
}
 
void readLightSensors() {
    lightMiddleVal = analogRead( lightMiddlePort );
    lightLeftVal = analogRead( lightLeftPort );
    lightRightVal = analogRead( lightRightPort );
    String left = "Left: ";
    String middle = ", Middle: ";
    String right = ", Right: ";
    String lightOutput = left + lightLeftVal + middle + lightMiddleVal + right + lightRightVal;
    //Serial.println( lightOutput );
    
    middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
    middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
    rightIsBlack = (lightRightVal > lowerRightBoundBlack);
    rightIsWhite = ( lightRightVal < upperRightBoundWhite );
    leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
    leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
    
    foundVertex = ( leftIsWhite && middleIsWhite && rightIsWhite );
}


void readDistanceSensors() {
  int counter = 0;
  while (counter < 5) {
    avgLeftDistance = avgLeftDistance + analogRead(A3);
    avgMiddleDistance = avgMiddleDistance + analogRead(A5);
    avgRightDistance = avgRightDistance + analogRead(A4);
    counter+=1;
  }
  LeftDistance = avgLeftDistance/5;
  MiddleDistance = avgMiddleDistance/5;
  RightDistance = avgRightDistance/5;
  avgLeftDistance = 0;
  avgMiddleDistance = 0;
  avgRightDistance = 0;
}


void Straight(){
  readDistanceSensors();
  //case 1, follow line straight, middle should see white, left/right on black.
    if ( middleIsWhite && leftIsBlack && rightIsBlack ) {
      goStraight();
    }
    //case 2, correct left, because I see left white.
    if ( leftIsWhite && rightIsBlack ) {
      correctLeft();
    }
    //case 3, correct right, because I see right white.
    if ( rightIsWhite && leftIsBlack ) {
      correctRight();
    }
}


void loop() {
  // put your main code here, to run repeatedly:
  readLightSensors();
   // put your main code here, to run repeatedly:
  readDistanceSensors();
  
  String left = "left: ";
  String middle = "     middle:";
  String right = "        right: ";
//  /Serial.println( left + LeftDistance + middle + MiddleDistance + right + RightDistance );
  //Serial.println( middle + MiddleDistance );

  if( !foundVertex ){
      Straight();
    }
  
  else { //found vertex
    Serial.println(MiddleDistance);
    //Serial.println( "found the vertex" );
    //delay( 200 );
    if( MiddleDistance > 170) {
      if (LeftDistance > 170) {
        goRight();
        foundVertex = false;
      }
      else if (RightDistance > 170) {
        goLeft();
        foundVertex = false;
      }
      //Serial.println( "got right" );
      //goStop();
//      delay( 100 );
      else {
        goLeft   ();
        foundVertex = false; 
      }
    }
    else{
      //goStop();
      //Serial.println( "got straight" );
      //delay(100);
//      Straight();
      goStraight();
      foundVertex = false;
    }
  }
}


void goRight() {
  correctRight();
  Serial.println( "turning right" );
  delay( 300 );
  readLightSensors();
  readDistanceSensors();
  while( !( rightIsBlack && leftIsBlack && middleIsWhite ) ) {
    servoLeft.write( 120 );
    servoRight.write( 90 );
    //Serial.println( "turning right" );
    readLightSensors();
  }
}


void goLeft() {
  correctLeft();
  Serial.println( "turning left" );
  delay( 300 );
  readLightSensors();
  while( !( rightIsBlack && leftIsBlack && middleIsWhite ) ) {
    servoLeft.write( 90 );
    servoRight.write( 60 );
    Serial.println( "turning left" );
    readLightSensors();
  }
}


void goStop() {
     servoLeft.write( 90 );
     servoRight.write( 90 );
     Serial.println( "going stopped" );
}


void goStraight() {
     servoLeft.write( 95 );
     servoRight.write( 85 );
     Serial.println( "going straight" );
}


void correctLeft(){
  servoLeft.write( 90 ); //stop one wheel
  servoRight.write( 60 );
  Serial.println( "correcting left" );
}


void correctRight(){
  servoLeft.write( 120 );
  servoRight.write( 90 ); //stop one wheel
  Serial.println( "correcting right" );
}
