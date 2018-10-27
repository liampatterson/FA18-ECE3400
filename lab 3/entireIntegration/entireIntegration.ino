#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#define BLUE_LED 4
#define RED_LED 2
#define WHITE_LED 1
#define GREEN_LED 0

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
int upperMiddleBoundWhite = 550;
int lowerMiddleBoundWhite = 0;

int upperMiddleBoundBlack = 1000;
int lowerMiddleBoundBlack = 550;

//RIGHT
int upperRightBoundWhite = 550;
int lowerRightBoundWhite = 0;

int upperRightBoundBlack = 1000;
int lowerRightBoundBlack = 550;

////LEFT
int upperLeftBoundWhite = 550;
int lowerLeftBoundWhite = 0 ; 

int upperLeftBoundBlack = 1000;
int lowerLeftBoundBlack = 550;

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

int muxOut = A3;

int counter = 0;
int average = 0;

boolean hasStarted = false;

// servo objects
Servo servoLeft;
Servo servoRight;

// to be used for mux digital inputs
int S2 = 5;
int S1 = 6;
int S0 = 7;
// Chn 000 left wall sensor
// Chn 001 middle wall sensor
// Chn 010 right wall sensor

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 115200 );
  ledSetup();
  //servoSetup();
  //to be used for mux digital inputs
  pinMode(4, OUTPUT); //enable
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);
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

int counterLight = 0;
int avgLightLeft = 0;
int avgLightRight = 0;
int avgLightMiddle = 0;

void readLightSensors() {
  while (counterLight < 3) {
    // Serial.println("got here");
     avgLightMiddle= avgLightMiddle + analogRead( lightMiddlePort );
     avgLightLeft= avgLightLeft+analogRead( lightLeftPort );
     avgLightRight= avgLightRight+ analogRead( lightRightPort );
     counterLight = counterLight + 1;
  }
  counterLight = 0;
  lightMiddleVal = avgLightMiddle/3;
  //Serial.println("hello");
  lightLeftVal = avgLightLeft/3;
  lightRightVal = avgLightRight/3;
//  String left = "Left: ";
//  Serial.println(left);
//  Serial.println(lightLeftVal);
//  String middle = ", Middle: ";
//  Serial.println(middle);
//  Serial.println(lightMiddleVal);
//  String right = ", Right: ";
//  Serial.println(right);
//  Serial.println(lightRightVal);
//  String lightOutput = left + lightLeftVal + middle + lightMiddleVal + right + lightRightVal;
//  Serial.println( lightOutput );
  
  middleIsWhite = (lightMiddleVal < upperMiddleBoundWhite );
  middleIsBlack = (lightMiddleVal > lowerMiddleBoundBlack);
  rightIsBlack = (lightRightVal > lowerRightBoundBlack);
  rightIsWhite = ( lightRightVal < upperRightBoundWhite );
  leftIsBlack = (lightLeftVal > lowerLeftBoundBlack);
  leftIsWhite = (lightLeftVal < upperLeftBoundWhite );

  foundVertex = ( leftIsWhite && middleIsWhite && rightIsWhite );

  avgLightLeft = 0;
  avgLightRight = 0;
  avgLightMiddle = 0;
}


void readDistanceSensors() {
  int counter = 0;
  avgLeftDistance = 0;
  avgMiddleDistance = 0;
  avgRightDistance = 0;
  digitalWrite(4, LOW);
  while (counter < 5) {
    chooseChannel0();
    avgLeftDistance = avgLeftDistance + analogRead(muxOut);    
    chooseChannel1();
    avgMiddleDistance = avgMiddleDistance + analogRead(muxOut);
    chooseChannel2();
    avgRightDistance = avgRightDistance + analogRead(muxOut);
    counter += 1;
  }
  digitalWrite(4, HIGH);
  LeftDistance = avgLeftDistance / 5;
  MiddleDistance = avgMiddleDistance / 5;
  RightDistance = avgRightDistance / 5;
  String l = "left dist ";
  String r = "  right dist ";
  String m = "  middle dist ";
  Serial.println(l+LeftDistance+m+MiddleDistance+r+RightDistance);
//  avgLeftDistance = 0;
//  avgMiddleDistance = 0;
//  avgRightDistance = 0;
}

void chooseChannel0() {
  digitalWrite(S2, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
}

void chooseChannel1() {
  digitalWrite(S2, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
}

void chooseChannel2() {
  digitalWrite(S2, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S0, LOW);
}

void chooseChannel3() {
  digitalWrite(S2, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S0, HIGH);
}



void goRight() {
  correctRight();
  Serial.println( "turning right" );
  delay( 150 );
  readLightSensors();
  while ( !( rightIsBlack && leftIsBlack && middleIsWhite ) ) {
    servoLeft.write( 120 );
    servoRight.write( 90 );
    //Serial.println( "turning right" );
    readLightSensors();
  }
}


void goLeft() {
  correctLeft();
  Serial.println( "turning left" );
  delay( 150 );
  readLightSensors();
  while ( !( rightIsBlack && leftIsBlack && middleIsWhite ) ) {
    servoLeft.write( 90 );
    servoRight.write( 60 );
    Serial.println( "turning left" );
    readLightSensors();
  }
}


void goStop() {
  servoLeft.write( 90 );
  servoRight.write( 90 );
  delay(5000);
  Serial.println( "going stopped" );
}





void correctLeft() {
  servoLeft.write( 90 ); //stop one wheel
  servoRight.write( 60 );
  Serial.println( "correcting left" );
}


void correctRight() {
  servoLeft.write( 120 );
  servoRight.write( 90 ); //stop one wheel
  Serial.println( "correcting right" );
}

void doNotStart() {
  servoLeft.write( 90 );
  servoRight.write( 90 );
}

void Straight() {
  //case 1, follow line straight, middle should see white, left/right on black.
  if ( middleIsWhite && leftIsBlack && rightIsBlack ) {
    goStraight();
    Serial.println("not correcting");
  }
  //case 2, correct left, because I see left white.
  if ( leftIsWhite && rightIsBlack ) {
    correctLeft();
    Serial.println("correcting left");
  }
  //case 3, correct right, because I see right white.
  if ( rightIsWhite && leftIsBlack ) {
    correctRight();
    Serial.println("correcting right");
  }
}





int tempADCSRA;
int tempTIMSK0;
int tempADMUX;
int tempDIDR0;
boolean r;
boolean start;




boolean startSound(){
    tempADCSRA = ADCSRA;
    tempTIMSK0 = TIMSK0;
    tempADMUX = ADMUX;
    tempDIDR0 = DIDR0;    
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x44; // use adc4
    DIDR0 = 0x01; // turn off the digital input for adc0
    //while(1) { // reduces jitter
      //counter = counter+1;
     //cli();  // UDRE interrupt slows this way down on arduino1.0
      for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
        while(!(ADCSRA & 0x10)); // wait for adc to be ready
        ADCSRA = 0xf5; // restart adc
        byte m = ADCL; // fetch adc data
        byte j = ADCH;
        int k = (j << 8) | m; // form into an int
        k -= 0x0200; // form into a signed int
        k <<= 6; // form into a 16b signed int
        fft_input[i] = k; // put real data into even bins
        fft_input[i+1] = 0; // set odd bins to 0
      }
      fft_window(); // window the data for better frequency response
      fft_reorder(); // reorder the data before doing the fft
      fft_run(); // process the data in the fft
      fft_mag_log(); // take the output of the fft
      sei();
      //Serial.println("start");
      for (byte i = 0 ; i < FFT_N/2 ; i++) { 
        //Serial.println(fft_log_out[i]); // send out the data
      }
      //Serial.println(fft_log_out[5]);
      start = false;
      average = average + fft_log_out[5];
      String avg = "average ";
      //Serial.println(avg+average);
      if(counter == 5){ 
        average = average/5; 
        
        //Serial.println(avg+average);
        if(average > 150){
          Serial.println("****************************this is 6.6Hz"); 
          //goStop();
          //delay(5000);
          start = true;
        }
        else{
          start = false;
        }
        counter = 0;
        average = 0;
      }
     counter+=1; 
     //Serial.println(counter);
    //}
    ADCSRA = tempADCSRA;
    TIMSK0= tempTIMSK0;
    ADMUX = tempADMUX;
    DIDR0 = tempDIDR0;
    //goStraight();
    return start;
 }

 
boolean detectIR(){
    tempADCSRA = ADCSRA;
    tempTIMSK0 = TIMSK0;
    tempADMUX = ADMUX;
    tempDIDR0 = DIDR0;    
    TIMSK0 = 0; // turn off timer0 for lower jitter
    ADCSRA = 0xe5; // set the adc to free running mode
    ADMUX = 0x45; // use adc5
    DIDR0 = 0x01; // turn off the digital input for adc0
    //while(1) { // reduces jitter
      //counter = counter+1;
     // cli();  // UDRE interrupt slows this way down on arduino1.0
      for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
        while(!(ADCSRA & 0x10)); // wait for adc to be ready
        ADCSRA = 0xf5; // restart adc
        byte m = ADCL; // fetch adc data
        byte j = ADCH;
        int k = (j << 8) | m; // form into an int
        k -= 0x0200; // form into a signed int
        k <<= 6; // form into a 16b signed int
        fft_input[i] = k; // put real data into even bins
        fft_input[i+1] = 0; // set odd bins to 0
      }
      fft_window(); // window the data for better frequency response
      fft_reorder(); // reorder the data before doing the fft
      fft_run(); // process the data in the fft
      fft_mag_log(); // take the output of the fft
      sei();
      //Serial.println("start");
      for (byte i = 0 ; i < FFT_N/2 ; i++) { 
       // Serial.println(fft_log_out[i]); // send out the data
      }

      r = false;
      average = average + fft_log_out[42];
      String avg = "average ";
      //Serial.println(avg+average);
      if(counter == 5){ 
        average = average/5; 
        
        //Serial.println(avg+average);
        if(average > 120){
          Serial.println("****************************this is 6kHz"); 
          //goStop();
          //delay(5000);
          r = true;
        }
        else{
          r = false;
        }
        counter = 0;
        average = 0;
      }
     counter+=1; 
     //Serial.println(counter);
    //}
    ADCSRA = tempADCSRA;
    TIMSK0= tempTIMSK0;
    ADMUX = tempADMUX;
    DIDR0 = tempDIDR0;
    //goStraight();
    return r;
 }

void goStraight() {
  servoLeft.write( 95 );
  servoRight.write( 85 );
  Serial.println( "going straight" );
}

void loop() {
  // put your main code here, to run repeatedly:
  readLightSensors();
 
  // put your main code here, to run repeatedly:

//  String left = "left: ";
//  String middle = "     middle:";
//  String right = "        right: ";
//  Serial.println( left + LeftDistance + middle + MiddleDistance + right + RightDistance );
  //Serial.println( middle + MiddleDistance );
  if (!hasStarted) {
    //doNotStart();
    Serial.println("here not started");
    if (startSound()) {
      hasStarted = true;
      servoSetup();
    }
  }
  else {
    //readDistanceSensors();
//    if(detectIR()){
//      goStop();
//    }
    if ( !foundVertex ) {
      Straight();
    }
    else { //found vertex

      readDistanceSensors();

//      if ( MiddleDistance > 175) {
//        if (LeftDistance > 200) {
//          goRight();
//          foundVertex = false;
//          //Serial.println( "Left Wall" );
//        }
//        else if (RightDistance > 175) {
//          goLeft();
//          //Serial.println( "Right Wall" );
//          foundVertex = false;
//        }
//        //Serial.println( "got right" );
//        //goStop();
//        //      delay( 100 );
//        else {
//          goLeft();
//          //Serial.println( "Middle Wall" );
//          foundVertex = false;
//        }
//      }
//      else{
        Straight();
       // foundVertex = false;
//      }
    }
  }
}
