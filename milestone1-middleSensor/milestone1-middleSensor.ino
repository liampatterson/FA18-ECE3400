#include <Servo.h>
// light sensor port numbers
int lightLeftPort = A0;
int lightMiddlePort = A1;
int lightRightPort = A2;


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
int upperRightBoundWhite = 600;
int lowerRightBoundWhite = 100;

int upperRightBoundBlack = 900;
int lowerRightBoundBlack = 600;

////LEFT
int upperLeftBoundWhite = 600;
int lowerLeftBoundWhite = 0 ;

int upperLeftBoundBlack = 900;
int lowerLeftBoundBlack = 600;

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

int countLeftTurns = 0;
int countRightTurns = 0;

int turns = 0;

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
  
  if( !foundVertex ) {
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
  if( foundVertex ) {
    Serial.println( "found the vertex" );
    goStop();
    delay( 200 );
    if ( turns < 4 ) {
      goRight();
    }
    else if ( turns >= 4 ) {
      goLeft();
    }
    turns++;
    if ( turns >= 8 ) {
      turns = 0;
    }
  }
}

void servoSetup() {
  servoLeft.attach( 9 );
  servoRight.attach( 10 );
}

void readLightSensors() {
    lightMiddleVal = analogRead( lightMiddlePort );
    lightLeftVal = analogRead( lightLeftPort );
    lightRightVal = analogRead( lightRightPort );
    String left = "Left: ";
    String middle = ", Middle: ";
    String right = ", Right: ";
    String lightOutput = left + lightLeftVal + middle + lightMiddleVal + right + lightRightVal;
    Serial.println( lightOutput );
    
    middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
    middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
    rightIsBlack = (lightRightVal > lowerRightBoundBlack);
    rightIsWhite = ( lightRightVal < upperRightBoundWhite );
    leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
    leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
    
    foundVertex = ( leftIsWhite && middleIsWhite && rightIsWhite );
}


void goRight() {
  correctRight();
  Serial.println( "turning right" );
  delay( 300 );
  readLightSensors();
  while( !( rightIsBlack && leftIsBlack && middleIsWhite ) ) {
    servoLeft.write( 120 );
    servoRight.write( 90 );
    Serial.println( "turning right" );
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

//
//void goForward() {
//  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
//  rightIsBlack = (lightRightVal > lowerRightBoundBlack);
//  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
//  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
//  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
//  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
//  if(middleIsBlack && rightIsBlack && leftIsWhite){
//    servoLeft.write( 95 );
//    servoRight.write( 85 );
//    Serial.println( "going straight" );
//  }
//  if(leftIsBlack && middleIsBlack && rightIsBlack){
//    correctLeft();
//  }
//  if((middleIsWhite && leftIsBlack && rightIsBlack) || (leftIsBlack && middleIsBlack && rightIsWhite)){
//    correctRight();
//  }
//  if(leftIsWhite && middleIsWhite && rightIsWhite){
//    goRight();
//    
//  }
//}
//
//void goRight() {
//  servoLeft.write( 160 );
//  servoRight.write( 160 );
//  Serial.println( "turning right" );
//  delay(50);
//  readLightSensors();
//
//  
//  while(rightIsWhite && middleIsWhite){
//    servoLeft.write( 160 );
//    servoRight.write( 160 );
//    Serial.println( "turning right" );
//    readLightSensors();
//    middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
//    rightIsBlack = (lightRightVal > lowerRightBoundBlack);
//    rightIsWhite = ( lightRightVal < upperRightBoundWhite );
//    leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
//    leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
//    middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
//  }
//
//}
//
//void goLeft() {
//  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
//  rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
//  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
//  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
//  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
//  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );
//  
//  while(!leftIsWhite){
//    servoLeft.write( 0 );
//    servoRight.write( 0 );
//    Serial.println( "correcting left" );
//  }
//}
//

//
