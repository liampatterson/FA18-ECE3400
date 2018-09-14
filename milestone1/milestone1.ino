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
int lowerMiddleBoundWhite = 300;

// a vertex for the middle
int upperMiddleBoundVertexWhite = 300;
int lowerMiddleBoundVertexWhite = 100;

int upperMiddleBoundBlack = 900;
int lowerMiddleBoundBlack = 800;

//RIGHT
int upperRightBoundWhite = 750;
int lowerRightBoundWhite = 100;

int upperRightBoundBlack = 900;
int lowerRightBoundBlack = 750;

////LEFT
int upperLeftBoundWhite = 750;
int lowerLeftBoundWhite =0 ;

int upperLeftBoundBlack = 900;
int lowerLeftBoundBlack =700;

boolean middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
boolean middleIsVertex = (lightMiddleVal > lowerMiddleBoundVertexWhite && lightMiddleVal < upperMiddleBoundVertexWhite );
boolean rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
boolean rightIsWhite = ( lightRightVal < upperRightBoundWhite );
boolean leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
boolean leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
boolean middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );

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
//  servoLeft.write( 90 );
//  servoRight.write( 90 );
  goForward();
  // delay( 200 );
}

void servoSetup() {
  servoLeft.attach( 5 );
  servoRight.attach( 6 );
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
  middleIsVertex = (lightMiddleVal > lowerMiddleBoundVertexWhite && lightMiddleVal < upperMiddleBoundVertexWhite );
  rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );
  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack && lightMiddleVal < upperMiddleBoundBlack );
  if(middleIsBlack && rightIsBlack && leftIsWhite){
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
  if(leftIsWhite && middleIsWhite && rightIsWhite){
    goRight();
    
  }
}

void goSlightRight() {
  servoLeft.write( 180 );
  servoRight.write( 120 );
}

void goRight() {
  servoLeft.write( 180 );
  servoRight.write( 90 );
  delay(750);
}

void goSlightLeft() {
  servoLeft.write( 60 );
  servoRight.write( 0 );
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
}


//void nextStep(){
//  boolean middleIsWhite = (lightMiddleVal > lowerMiddleBoundWhite && lightMiddleVal < upperMiddleBoundWhite );
//  boolean middleIsVertex = (lightMiddleVal > lowerMiddleBoundVertexWhite && lightMiddleVal < upperMiddleBoundVertexWhite );
//  boolean rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
//  boolean rightIsWhite = (lightRightVal > lowerRightBoundWhite && lightRightVal < upperRightBoundWhite );
//  boolean leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
//  boolean leftIsWhite = (lightLeftVal > lowerLeftBoundWhite && lightLeftVal < upperLeftBoundWhite );
//  
//  
//  if (leftIsWhite  && middleIsVertex && rightIsWhite ){
//     goSlightRight(); 
//  } 
//  if (leftIsBlack && middleIsWhite && rightIsBlack){
//    goForward();    
//  } 
//}  

int countForwards = 0;
boolean goingRight = false;
//void figure8(){
//  boolean middleIsWhite = (lightMiddleVal > lowerMiddleBoundWhite && lightMiddleVal < upperMiddleBoundWhite );
//  boolean middleIsVertex = (lightMiddleVal > lowerMiddleBoundVertexWhite && lightMiddleVal < upperMiddleBoundVertexWhite );
//  boolean rightIsBlack = (lightRightVal > lowerRightBoundBlack && lightRightVal < upperRightBoundBlack );
//  boolean rightIsWhite = (lightRightVal > lowerRightBoundWhite && lightRightVal < upperRightBoundWhite );
//  boolean leftIsBlack = (lightLeftVal > lowerLeftBoundBlack && lightLeftVal < upperLeftBoundBlack );
//  boolean leftIsWhite = (lightLeftVal > lowerLeftBoundWhite && lightLeftVal < upperLeftBoundWhite );
//  
//  
//  if (leftIsWhite  && middleIsVertex && rightIsWhite ){
//     goSlightRight(); 
//  } 
//  if (leftIsBlack && middleIsWhite && rightIsBlack){
//    goForward();    
//  } 
//}
//void square(){
//  if (leftIsWhite  && rightIsBlack ){
//    goForward();
//  } 
//  if (leftIsWhite && rightIsWhite){
//    goSlightRight();    
//  } 
//}
