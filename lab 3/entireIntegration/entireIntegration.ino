#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft
#define BLUE_LED 4
#define RED_LED 2
#define WHITE_LED 1
#define GREEN_LED 0

#include <FFT.h> // include the library
#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


// BEGIN MAZE SIMULATION DEFINITIONS
    
    //CREATE MAZE
    /**
    
          __0____1____2__
       0 | tTR       tSB |
         |    ___________|
       1 |       R       |
         |___________    |
       2 |      tCR      |
         |_______________|
    
    
    */
    
    typedef struct {
      bool northWall;
      bool southWall;
      bool eastWall;
      bool westWall;
      bool treasureCircle;
      bool treasureTriangle;
      bool treasureSquare;
      bool treasureRed;
      bool treasureBlue;
      bool robotPresent;
    } mazeSquare;
    
    mazeSquare maze[3][3] {
      //{ { true, false, false, true, false, true, false, true }, { true, true, false, false }, { true, true, true, false, false, false, true, false, true } },
      { { true, false, false, true }, { true, true, false, false }, { true, true, true, false } },
      //{ { false, true, false, true }, { true, true, false, false, false, false, false, false, false, true }, { true, false, true, false } },
      { { false, true, false, true }, { true, true, false, false }, { true, false, true, false } },
      //{ { true, true, false, true }, { true, true, false, false, true, false, false, true }, {false, true, true, false} }  
      { { true, true, false, true }, { true, true, false, false }, {false, true, true, false} }  
    };
// END MAZE SIMULATION DEFINITIONS

// BEGIN BIT MASKS

    #define WALL_NORTH_PRESENT B10000000;
    #define WALL_EAST_PRESENT  B01000000;
    #define WALL_SOUTH_PRESENT B00100000;
    #define WALL_WEST_PRESENT  B00010000;
    
    #define TREASURE_NONE      B00000000;
    #define TREASURE_TRIANGLE  B00000100;
    #define TREASURE_CIRCLE    B00001000;
    #define TREASURE_SQUARE    B00001100;
    
    #define TREASURE_RED       B00000000;
    #define TREASURE_BLUE      B00000010;
    
    #define ROBOT_NOT_PRESENT  B11111110;
    #define ROBOT_PRESENT      B00000001;
    
    #define EXPLORED           B10000000;
    
// END BIT MASKS

// BEGIN light sensor port numbers
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
int S2 = 3;
int S1 = 4;
int S0 = 7;
// Chn 000 left wall sensor
// Chn 001 middle wall sensor
// Chn 010 right wall sensor


// RADIO CONSTANTS
    // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
    
    RF24 radio(9, 10);
    
    //
    // Topology
    //
    
    // Radio pipe addresses for the 2 nodes to communicate.
    const uint64_t pipes[2] = { 0x000000000ALL, 0x000000000BLL };
    
    //
    // Role management
    //
    // Set up role.  This sketch uses the same software for all the nodes
    // in this system.  Doing so greatly simplifies testing.
    //
    
    // The various roles supported by this sketch
    typedef enum { role_ping_out = 1, role_pong_back } role_e;
    
    // The debug-friendly names of those roles
    const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
    
    // The role of the current running sketch
    role_e role = role_ping_out;
    
    
    bool northWall;
    bool southWall;
    bool eastWall;
    bool westWall;
    bool treasureCircle;
    bool treasureTriangle;
    bool treasureSquare;
    bool treasureRed;
    bool treasureBlue;
    bool robotPresent;

// END RADIO CONSTANTS


/*** BEGIN MAIN CODE ***/
void setup( void )
{
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  ledSetup();
  radioSetup();
  //servoSetup();
  //to be used for mux digital inputs
  pinMode(2, OUTPUT); //enable
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  for ( int yVal = 0; yVal < 3; yVal++ ) {
    for ( int xVal = 0; xVal < 3; xVal++ ) {
      transmitSqData( xVal, yVal );
    }
  }
 
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

/*** END MAIN CODE ***/


/******* BEGIN HELPER FUNCTIONS ********/
void servoSetup( void )
{
  servoLeft.attach( 5 );
  servoRight.attach( 6 );
}

void ledSetup( void )
{
  pinMode( 0, OUTPUT ); //green LED
  pinMode( 1, OUTPUT ); //white LED
  pinMode( 2, OUTPUT ); //red LED
  pinMode( 4, OUTPUT ); //blue LED
}

int counterLight = 0;
int avgLightLeft = 0;
int avgLightRight = 0;
int avgLightMiddle = 0;

void readLightSensors( void )
{
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


void readDistanceSensors( void ) 
{
  int counter = 0;
  avgLeftDistance = 0;
  avgMiddleDistance = 0;
  avgRightDistance = 0;
  digitalWrite(2, LOW);
  while (counter < 5) {
    chooseChannel0();
    avgLeftDistance = avgLeftDistance + analogRead(muxOut);    
    chooseChannel1();
    avgMiddleDistance = avgMiddleDistance + analogRead(muxOut);
    chooseChannel2();
    avgRightDistance = avgRightDistance + analogRead(muxOut);
    counter += 1;
  }
  digitalWrite(2, HIGH);
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


// BEGIN MUX DEFINITIONS
void chooseChannel0( void )
{
  digitalWrite(S2, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, LOW);
}

void chooseChannel1( void )
{
  digitalWrite(S2, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S0, HIGH);
}

void chooseChannel2( void )
{
  digitalWrite(S2, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S0, LOW);
}

void chooseChannel3( void )
{
  digitalWrite(S2, LOW);
  digitalWrite(S1, HIGH);
  digitalWrite(S0, HIGH);
}
// END MUX DEFINITIONS 


// BEGIN MOVEMENT DEFINITIONS
      void goRight( void )
      {
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
      
      
      void goLeft( void ) 
      {
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
      
      
      void goStop( void )
      {
        servoLeft.write( 90 );
        servoRight.write( 90 );
        delay(5000);
        Serial.println( "going stopped" );
      }
      
      
      void correctLeft( void )
      {
        servoLeft.write( 90 ); //stop one wheel
        servoRight.write( 60 );
        Serial.println( "correcting left" );
      }
      
      
      void correctRight( void )
      {
        servoLeft.write( 120 );
        servoRight.write( 90 ); //stop one wheel
        Serial.println( "correcting right" );
      }
      
      void doNotStart( void )
      {
        servoLeft.write( 90 );
        servoRight.write( 90 );
      }
      
      void Straight( void )
      {
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

      void goStraight( void )
      {
        servoLeft.write( 95 );
        servoRight.write( 85 );
        Serial.println( "going straight" );
      }
// END MOVEMENT DEFINITIONS


int tempADCSRA;
int tempTIMSK0;
int tempADMUX;
int tempDIDR0;
boolean r;
boolean start;


boolean startSound( void )
{
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

 
boolean detectIR( void )
{
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


/*********** RADIO STUFF BELOW THIS LINE **********/

/*********** TO BE RUN IN setup() ***********/
void radioSetup( void )
{
    printf_begin();
  //printf("\n\rRF24/examples/GettingStarted/\n\r");
  //printf("ROLE: %s\n\r", role_friendly_name[role]);
  //printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm.
  radio.setPALevel(RF24_PA_MAX);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_2MBPS);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //

  // This simple sketch opens two pipes for these two nodes to communicate
  // back and forth.
  // Open 'our' pipe for writing
  // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)

  if ( role == role_ping_out )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1, pipes[0]);
  }

  //
  // Start listening
  //

  radio.startListening();

  //
  // Dump the configuration of the rf unit for debugging
  //

  //radio.printDetails();
  Serial.println( "reset" );
  transmit( 0xFF );
}


/* MAIN TRANSMIT FUNCTION, CALL ON TRAVERSING TO 
 *  NEW SQUARE
 */
void transmitSqData( int xVal, int yVal )
{
    assignGlobalMaze( xVal, yVal ); //FOR SIMULATION ONLY !!!!!!!!!!
    byte coordinates = xVal << 4 | yVal ;
    byte first = 0x1b;
    byte second = 0x2b;
    byte coords = 0xcc;
    byte firstByte = 0;
    firstByte = checkWalls( firstByte );
    firstByte = checkTreasure( firstByte );
    firstByte = checkIr( firstByte );
    byte secondByte = EXPLORED;

    transmit( first );
    delay( 250 );
    transmit( firstByte );
    delay( 250 );
    transmit( second );
    delay( 250 );
    transmit( secondByte);
    delay( 250 );
    transmit( coords );
    delay( 250 );
    transmit( coordinates );
    delay( 2000 );
}


// FOR SIMULATION ONLY !!!!!!!!!!
void assignGlobalMaze ( int xVal, int yVal )
{
  northWall = maze[ yVal ][ xVal ].northWall;
  southWall = maze[ yVal ][ xVal ].southWall;
  eastWall = maze[ yVal ][ xVal ].eastWall;
  westWall = maze[ yVal ][ xVal ].westWall;
  treasureCircle  = maze[ yVal ][ xVal ].treasureCircle;
  treasureTriangle = maze[ yVal ][ xVal ].treasureTriangle;
  treasureSquare = maze[ yVal ][ xVal ].treasureSquare;
  treasureRed = maze[ yVal ][ xVal ].treasureRed;
  treasureBlue = maze[ yVal ][ xVal ].treasureBlue;
  robotPresent = maze[ yVal ][ xVal ].robotPresent;
}



/** USAGE FOR THE BELOW THREE PROGRAMS
 *  checkWalls checks for walls
 *    north/south/east/westWall are global variables
 *    please assign them values when checking in each square
 *  checkTreasure checks the value of treasure
 *    again, they're global variables, assign them 
 *    values and this will byte encode them
 *  checkIr does the same thing for other robots
 *    again it's a global variable
 */
byte checkWalls( byte firstByte )
{
  if ( northWall ) firstByte |= WALL_NORTH_PRESENT;
  if ( southWall ) firstByte |= WALL_SOUTH_PRESENT;
  if ( eastWall ) firstByte |= WALL_EAST_PRESENT;
  if ( westWall ) firstByte |= WALL_WEST_PRESENT;
  return firstByte;
}


byte checkTreasure( byte firstByte )
{
  if ( treasureTriangle ) firstByte |= TREASURE_TRIANGLE;
  if ( treasureCircle ) firstByte |= TREASURE_CIRCLE;
  if ( treasureSquare ) firstByte |= TREASURE_SQUARE;
  if ( treasureRed ) firstByte |= TREASURE_RED;
  if ( treasureBlue ) firstByte |= TREASURE_BLUE;
  return firstByte;
}


byte checkIr( byte firstByte ) 
{
  if ( robotPresent ) {
    firstByte |= ROBOT_PRESENT; 
    }
  else if ( !robotPresent ) firstByte &= ROBOT_NOT_PRESENT;
  return firstByte;
}


void transmit( byte payload ) 
{
  // Ping out role.  Repeatedly send the current time
  //

  //if ( role == role_ping_out )
  //{
    // First, stop listening so we can talk.
    radio.stopListening();
    
    printf( "Now sending %x...", payload );
    bool ok = radio.write( &payload, sizeof(unsigned char) );
    if ( ok )
      printf( "ok..." );
    else
      printf( "failed.\n\r" );

    // Now, continue listening
    radio.startListening();

    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 1000 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned char rxPayload;
      radio.read( &rxPayload, sizeof(unsigned char) );
      if ( rxPayload == payload ) {
        // Spew it
        printf("Received correct data %x, RT delay: %lu\n\r", rxPayload, millis() - started_waiting_at);
      }
      else {
        printf("That's not what I sent, received %x, RT delay: %lu\n\r", rxPayload, millis() - started_waiting_at);
      }
    }

    // Try again 1s later
    delay(1000);
  //}
}
