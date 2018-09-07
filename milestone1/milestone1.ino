
// light sensor port numbers
int lightLeftPort = A0;
int lightRightPort = A1;

// light sensor read values
int lightLeftVal = 0;
int lightRightVal = 0;

int upperBoundWhite = 800;
int lowerBoundWhite = 650;

int upperBoundBlack = 999;
int lowerBoundBlack = 900;

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
<<<<<<< HEAD
  nextStep();
  // delay( 200 );
=======
  delay( 200 );
>>>>>>> fa20dd9b4477fc76760f4521b3c5d348eee20211
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
    lightLeftVal = analogRead( lightLeftPort );
    lightRightVal = analogRead( lightRightPort );
    String left = "Left: ";
    String right = ", Right: ";
    String lightOutput = left + lightLeftVal + right + lightRightVal;
    Serial.println( lightOutput );

}

void nextStep(){
  if (lightLeftVal = %white && lightRightVal = %black){
    goForward();
  } 
  if (lightLeftVal = %white && lightRightVal = %white){
    goSlightRight();    
  } 
  if (lightLeftVal = %black && lightRightVal = &black){
    goSlightLeft();
  }
  

}

void nextStep(){
  boolean leftIsBlack = (lightLeftVal > lowerBoundBlack && lightLeftVal < upperBoundBlack );
  boolean leftIsWhite = (lightLeftVal > lowerBoundWhite && lightLeftVal < upperBoundWhite );
  boolean rightIsBlack = (lightRightVal > lowerBoundBlack && lightRightVal < upperBoundBlack );
  boolean rightIsWhite = (lightRightVal > lowerBoundWhite && lightRightVal < upperBoundWhite );
  
  if (leftIsWhite  && rightIsBlack ){
    goForward();
  } 
  if (leftIsWhite && rightIsWhite){
    goSlightRight();    
  } 
  if (leftIsBlack && rightIsBlack){
    goSlightLeft();
  }
}  
