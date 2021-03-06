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
int upperMiddleBoundWhite = 750;
int lowerMiddleBoundWhite = 0;

int upperMiddleBoundBlack = 900;
int lowerMiddleBoundBlack = 800;

//RIGHT
int upperRightBoundWhite = 750;
int lowerRightBoundWhite = 100;

int upperRightBoundBlack = 900;
int lowerRightBoundBlack = 750;

////LEFT
int upperLeftBoundWhite = 750;
int lowerLeftBoundWhite =65 ;

int upperLeftBoundBlack = 900;
int lowerLeftBoundBlack =700;

// a vertex for the left
int upperLeftBoundVertexWhite = 65;
int lowerLeftBoundVertexWhite = 0;

boolean middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
boolean leftIsVertex = (lightLeftVal < upperLeftBoundVertexWhite );
boolean rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
boolean rightIsWhite = ( lightRightVal < upperRightBoundWhite );
boolean leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
boolean leftIsWhite = (lightLeftVal>lowerLeftBoundWhite && lightLeftVal < upperLeftBoundWhite );
boolean middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );

int countLeftTurns = 0;
int countRightTurns = 0;

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

//  servoLeft.write( 90 );
//  servoRight.write( 90 );
  readLightSensors();
  if (middleIsWhite && rightIsWhite && leftIsVertex){
    goRight();
   }
  else{
    goForward();
  }

}

void servoSetup() {
  servoLeft.attach( 9 );
  servoRight.attach( 10 );
}

void correctLeft(){
  servoLeft.write( 90 ); //stop one wheel
  servoRight.write( 50 );
  Serial.println( "correcting left" );
}

void correctRight(){
  servoLeft.write( 130 );
  servoRight.write( 90 ); //stop one wheel
  Serial.println( "correcting right" );
}


void goForward() {
  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
  rightIsBlack = (lightRightVal > lowerRightBoundBlack);
  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
  leftIsVertex = (lightLeftVal < upperLeftBoundVertexWhite );
  if(middleIsWhite && rightIsBlack && leftIsBlack){
    servoLeft.write( 95 );
    servoRight.write( 85 );
    Serial.println( "going straight" );
  }
  if(leftIsBlack && middleIsBlack && rightIsBlack){
    correctLeft();
  }
  if((middleIsWhite && leftIsBlack && rightIsBlack) || (leftIsBlack && middleIsBlack && rightIsWhite)){
    correctRight();
  }
  if(leftIsVertex && middleIsWhite && rightIsWhite){
    goRight();
    
  }
}

void goRight() {
  servoLeft.write( 95 );
  servoRight.write( 85 );
  delay(100);
  servoLeft.write( 100 );
  servoRight.write( 100 );
  Serial.println( "turning right" );
  delay(300);
  readLightSensors();
  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
  rightIsBlack = (lightRightVal > lowerRightBoundBlack);
  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
  leftIsVertex = (lightLeftVal < upperLeftBoundVertexWhite );
  while(!(rightIsBlack && middleIsBlack && leftIsWhite)){
    servoLeft.write( 100 );
    servoRight.write( 100 );
    Serial.println( "turning right" );
    readLightSensors();
    middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
    rightIsBlack = (lightRightVal > lowerRightBoundBlack);
    rightIsWhite = ( lightRightVal < upperRightBoundWhite );
    leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
    leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
    middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
  }

}

void goLeft() {
  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
  rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );
  
  while(!leftIsWhite && !middleIsWhite && ! rightIsWhite){
    servoLeft.write( 0 );
    servoRight.write( 0 );
    Serial.println( "correcting left" );
  }
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
}

//void figureEight() {
//  //note: this is code for figure eight implementation, but it should be placed in loop when used in order to update sensor values.
//  if (!(leftIsWhite && rightIsWhite && middleIsWhite)){
//    goForward();
//   }
//  else{
//    if (countRightTurns < 3){
//      goRight();
//      countRightTurns++;
//    }
//    else if (countLeftTurns < 3){
//      goLeft();
//      countLeftTurns++;  
//    }
//    else if (countRightTurns == 3){
//      goForward();
//      countRightTurns++;
//    }
//    else{
//      goForward();
//      countRightTurns = 0;
//      countLeftTurns = 0;
//    }
//  }
//}
