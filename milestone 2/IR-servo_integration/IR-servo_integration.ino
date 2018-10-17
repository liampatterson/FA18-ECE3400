/*
fft_adc_serial.pde
guest openmusiclabs.com 7.7.14
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.
*/
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

int countLeftTurns = 0;
int countRightTurns = 0;

int turns = 0;

// servo objects
Servo servoLeft;
Servo servoRight;

void servoSetup() {
  servoLeft.attach( 9 );
  servoRight.attach( 10 );
}

void setup() {
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  servoSetup();
}

int counter = 0;
int average = 0;

void loop() {
  while(1) { // reduces jitter
    counter = counter+1;
    cli();  // UDRE interrupt slows this way down on arduino1.0
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
    Serial.println("start");
    for (byte i = 0 ; i < FFT_N/2 ; i++) { 
      Serial.println(fft_log_out[i]); // send out the data
    }
    average = average + fft_log_out[5];
    //Serial.println(fft_log_out[5]);
    if(counter == 5){ 
      average = average/5;
      Serial.println(average); 
      if(average > 115){
        //Serial.println("this is 660"); 
        digitalWrite(6,HIGH);
      }
  
  //    if (fft_log_out[42] > 85) {
  //      Serial.println("this is 6kHz");
  //      digitalWrite(6, LOW);
  //      digitalWrite(7, HIGH);
  //    }
  //
  //    else if (fft_log_out[122] > 60) {
  //      Serial.println("this is 18kHz");
  //      digitalWrite(6, HIGH);
  //      digitalWrite(7, LOW);
  //    }
      else{
        Serial.println("diff frequency"); 
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
      }
      counter = 0;
      average = 0;
    }
  }
}
