#define LOG_OUT 1 // use the log output function
#define FFT_N 64   // set to 256 point fft

#include <FFT.h> // include the library
#include <Servo.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <StackArray.h>


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

/*typedef struct {
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
  { { true, false, false, true }, { true, true, false, false }, { true, true, true, false } },
  { { false, true, false, true }, { true, true, false, false }, { true, false, true, false } },
  { { true, true, false, true }, { true, true, false, false }, {false, true, true, false} }
  };*/

typedef enum { north_up = 0, north_left = 1, north_back = 2, north_right = 3 } Orientation; //

/**         __360º__
          /  N-Up    \
         /            \
   270º | N-Rgt   N-Lft| 090º
         \            /
          \__N_back__/
              180º
*/
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

#define WALL_THRESHOLD     230

// END BIT MASKS

// BEGIN light sensor port numbers
const int lightLeftPort = A0;
const int lightMiddlePort = A1;
const int lightRightPort = A2;

int avgRightDistance = 0;
int avgMiddleDistance = 0;
int avgLeftDistance = 0;

// light sensor read values
int lightMiddleVal = 0;
int lightRightVal = 0;
int lightLeftVal = 0;

//#define   UPPER_MIDDLE_BOUND_WHITE   550;
//#define   LOWER_MIDDLE_BOUND_WHITE   0;
//#define   UPPER_MIDDLE_BOUND_BLACK   1000;
//#define   LOWER_MIDDLE_BOUND_BLACK   550;
//#define   UPPER_RIGHT_BOUND_WHITE   550;
//#define   LOWER_RIGHT_BOUND_WHITE   0;
//#define   UPPER_RIGHT_BOUND_BLACK   1000;
//#define   LOWER_RIGHT_BOUND_BLACK   550;
//#define   UPPER_LEFT_BOUND_WHITE   550;
//#define   LOWER_LEFT_BOUND_WHITE   0;
//#define   UPPER_LEFT_BOUND_BLACK   1000;
//#define   LOWER_LEFT_BOUND_BLACK   550;

//MIDDLE
const int UPPER_MIDDLE_BOUND_WHITE = 550;
const int LOWER_MIDDLE_BOUND_WHITE = 0;

const int UPPER_MIDDLE_BOUND_BLACK = 1000;
const int LOWER_MIDDLE_BOUND_BLACK = 550;

//RIGHT
const int UPPER_RIGHT_BOUND_WHITE = 550;
const int LOWER_RIGHT_BOUND_WHITE = 0;

const int UPPER_RIGHT_BOUND_BLACK = 1000;
const int LOWER_RIGHT_BOUND_BLACK = 550;

////LEFT
const int UPPER_LEFT_BOUND_WHITE = 550;
const int LOWER_LEFT_BOUND_WHITE = 0 ;

const int UPPER_LEFT_BOUND_BLACK = 1000;
const int LOWER_LEFT_BOUND_BLACK = 550;


// a vertex for the left
//int upperLeftBoundVertexWhite = 65;
//int lowerLeftBoundVertexWhite = 0;

boolean middleIsWhite = (lightMiddleVal < UPPER_MIDDLE_BOUND_WHITE );
boolean rightIsBlack = (lightRightVal > LOWER_RIGHT_BOUND_BLACK && lightRightVal < UPPER_RIGHT_BOUND_BLACK );
boolean rightIsWhite = ( lightRightVal < UPPER_RIGHT_BOUND_WHITE );
boolean leftIsBlack = (lightLeftVal > LOWER_LEFT_BOUND_BLACK && lightLeftVal < UPPER_LEFT_BOUND_BLACK );
boolean leftIsWhite = (lightLeftVal < UPPER_LEFT_BOUND_WHITE );
boolean middleIsBlack = (lightMiddleVal > LOWER_MIDDLE_BOUND_BLACK && lightMiddleVal < UPPER_MIDDLE_BOUND_BLACK );

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

int caseVariable = B000; //binary value where leftmost bit is left wall, then middle is middle wall, then rightmost bit is right wall

// servo objects
Servo servoLeft;
Servo servoRight;

// to be used for mux digital inputs
int S2 = 2;
int S1 = 3;
int S0 = 4;
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

//
bool northWall = true;
bool southWall;
bool eastWall;
bool westWall = true;
bool treasureCircle;
bool treasureTriangle;
bool treasureSquare;
bool treasureRed;
bool treasureBlue;
bool robotPresent;
//
Orientation orientation = north_back;
int xVal = 0;
int yVal = 0;
int turn = 0;

// END RADIO CONSTANTS

//variables for DFS
byte visited[81];
StackArray<byte> stack;
byte startNode = B00000001;
byte current;
boolean didSomething;

byte possibleForwardNode;
byte possibleBackNode;
byte possibleLeftNode;
byte possibleRightNode;

bool sawRight = false;
bool sawMiddle = false;
bool sawLeft = false;


/*** BEGIN MAIN CODE ***/
void setup( void )
{
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  ledSetup();
  radioSetup();
  readDistanceSensors();

  transmitSqData(0, 0);
  // Serial.println("completed radio setup***************************");
  //to be used for mux digital inputs
  pinMode(S2, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S0, OUTPUT);


  stack.push(startNode);

  randomSeed (analogRead (0));

}


/*** BEGIN HELPER FUNCTIONS FOR DFS ***/

/*Helper function to add a new byte to visited[]*/

//end variables for DFS
void add(byte b) {
  for (int k = 1; k < 81; k++)
  {
    if (visited[k] == B0) {
      visited[k] = b; //add it to the end of the array
      break;
    }
  }
}

boolean in(byte b) {
  for ( int i = 0; i < 81; i++ ) {
    if (visited[i] == b) {
      return false;
    }
  }
  return true;
}

//assumes coordinates are in format char xVal,yVal
void decodePossibleSets( Orientation orientation, int xValtemp, int yValtemp ) {
  //00010001 is coordinate (1,1)
  //00100001 is coordinate (2,1)
  //  int xValtemp = ( currentCoords >> 4 );
  //  int yValtemp = ( currentCoords && B00001111 );

  switch ( orientation ) {
    case north_up:
      possibleForwardNode = intToByte( xValtemp, yValtemp - 1 );
      possibleBackNode = intToByte( xValtemp, yValtemp + 1 );
      possibleLeftNode = intToByte( xValtemp - 1, yValtemp );
      possibleRightNode = intToByte( xValtemp + 1, yValtemp );
      break;
    case north_left:
      possibleForwardNode = intToByte( xValtemp + 1, yValtemp );
      possibleBackNode = intToByte( xValtemp - 1, yValtemp );
      possibleLeftNode = intToByte( xValtemp, yValtemp - 1 );
      possibleRightNode = intToByte( xValtemp, yValtemp + 1 );
      break;
    case north_back:
      possibleForwardNode = intToByte( xValtemp, yValtemp + 1 );
      possibleBackNode = intToByte( xValtemp, yValtemp - 1 );
      possibleLeftNode = intToByte( xValtemp + 1, yValtemp );
      possibleRightNode = intToByte( xValtemp - 1, yValtemp );
      break;
    case north_right:
      possibleForwardNode = intToByte( xValtemp - 1, yValtemp );
      possibleBackNode = intToByte( xValtemp + 1, yValtemp );
      possibleLeftNode = intToByte( xValtemp, yValtemp + 1 );
      possibleRightNode = intToByte( xValtemp, yValtemp - 1 );
      break;
    default:
      break;
  }
}

byte intToByte( int xVal, int yVal ) {
  byte returnVal;
  returnVal = (( xVal << 4 ) | ( yVal % 15));
  return returnVal;
}
/*** END HELPER FUNCTIONS FOR DFS ***/
boolean tempFoundVertex;

void loop() {
  // Serial.println("got inside loop");
  readLightSensors();
  if (!hasStarted) {
    // Serial.println("not started");
    while (!hasStarted) {
      int m = 0;
      for (int x = 5; x >= 0; x--) {
        delay(250);
        if ( digitalRead(8) == HIGH ) {
          Serial.println(digitalRead(8));
          m++;
        }
      }
      if (m > 5) {
        hasStarted = true;
        Serial.println("8 is high");
        servoSetup();
      }
    }

    //    if (startSound()) {
    //      hasStarted = true;
    //      servoSetup();
    //    }
  }
  else {

    if ( !foundVertex ) { //no vertex, go straight
      Straight();
    }
    else { //found vertex
      turn = 0;
      tempFoundVertex = true; //found a vertex for now
      //    if (detectIR()) {
      //        goStop();
      //        delay(2000);
      //        Straight();
      //    }
      //     if (detectIR() && !foundVertex) {
      //      turnAround();
      //      if ( orientation > 1 ) {
      //        orientation = orientation - 2;
      //      }
      //      else {
      //        orientation = 2;
      //      }
      //      stack.pop();
      //      stack.push(current);
      //    }
      //    else if (detectIR()) {
      //      current = stack.pop();
      //      xVal = ( current >> 4 );
      //      yVal = ( current & B00001111 );
      //      if (in(current)) { //if current is not in visited
      //        add(current); //set current coordinate to be visited, TODO add an add function
      //      }
      //      decodePossibleSets( orientation, xVal, yVal  );
      //      turnAround();
      //      turn = 3;
      //      tempFoundVertex = false;
      //      orientRobot( caseVariable, turwn );
      //      transmitSqData( yVal, xVal );
      //      stack.push(possibleBackNode);
      //    }
      //readDistanceSensors();
      goStraight();
      delay(500);
      goStop();
      delay(300);
      readDistanceSensors();
      delay(100);
      readDistanceSensors();
      delay(100);
      didSomething = false;
      current = stack.pop();
      //if (current != B0) {
      //        coordinateCalculation( orientation );
      //}
      xVal = ( current >> 4 );
      yVal = ( current & B00001111 );
      if (in(current)) { //if current is not in visited
        add(current); //set current coordinate to be visited, TODO add an add function
      }
      decodePossibleSets( orientation, xVal, yVal  );
      //      Serial.println(caseVariable);
      //      Serial.print("visited list ");
      //      for (int y = 0; y < 81; y++) {
      //        Serial.print(visited[y] + " ");
      //      }
      //      printf(visited);
      //check the unvisited nodes first
      switch (caseVariable) {
        case B000:
          if (in(possibleForwardNode)) {
            goStraight();
            Straight();
            turn = 0;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleForwardNode);
          }
          else if (in(possibleLeftNode)) {
            goLeft();
            turn = 1;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleLeftNode);
          }
          else if (in(possibleRightNode)) {
            goRight();
            turn = 2;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleRightNode);
          }
          break;
        case B101:
          if (in(possibleForwardNode)) {
            goStraight();
            turn = 0;
            Straight();
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleForwardNode);
          }
          break;
        case B001:
          if (in(possibleForwardNode)) {
            goStraight();
            Straight();
            turn = 0;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleForwardNode);
          }
          else if (in(possibleLeftNode)) {
            goLeft();
            turn = 1;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleLeftNode);
          }
          break;
        case B100:
          if (in(possibleForwardNode)) {
            goStraight();
            Straight();
            turn = 0;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleForwardNode);
          }
          else if (in(possibleRightNode)) {
            goRight();
            turn = 2;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleRightNode);
          }
          break;
        case B011:
          if (in(possibleLeftNode)) {
            goLeft();
            turn = 1;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleLeftNode);
          }
          break;
        case B010:
          if (in(possibleLeftNode)) {
            goLeft();
            turn = 1;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleLeftNode);
          }
          else if (in(possibleRightNode)) {
            goRight();
            turn = 2;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleRightNode);
          }
          break;
        case B110:
          if (in(possibleRightNode)) {
            goRight();
            turn = 2;
            didSomething = true;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleRightNode);
          }
          break;
        default:
          break;
      }
      if (!didSomething) {
        //delay(6000);
        int randtemp = 0;
        //now check everything around you if you have to go to a visited node
        switch (caseVariable) {
          case B101:
            goStraight();
            Straight();
            turn = 0;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleForwardNode);
            break;
          case B001:
            randtemp = random(2);
            if (randtemp == 0) {
              goStraight();
              Straight();
              turn = 0;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleForwardNode);
            }
            else {
              goLeft();
              turn = 1;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleLeftNode);
            }
            break;
          case B100:
            randtemp = random(2);
            if (randtemp == 0) {
              goStraight();
              Straight();
              turn = 0;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleForwardNode);
            }
            else {
              goRight();
              turn = 2;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleRightNode);
            }
            break;
          case B000:
            randtemp = random(3);
            if (randtemp == 0) {
              goStraight();
              Straight();
              turn = 0;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleForwardNode);
            }
            else if (randtemp == 1) {
              goLeft();
              turn = 1;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleLeftNode);
            }
            else {
              goRight();
              turn = 2;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleRightNode);
            }
            break;
          case B011:
            goLeft();
            turn = 1;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleLeftNode);
            break;
          case B010:
            randtemp = random(2);
            if (randtemp == 0) {
              goLeft();
              turn = 1;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleLeftNode);
            }
            else {
              goRight();
              turn = 2;
              tempFoundVertex = false;
              orientRobot( caseVariable, turn );
              transmitSqData( yVal, xVal );
              stack.push(possibleRightNode);
            }
            break;
          case B110:
            goRight();
            turn = 2;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleRightNode);
            break;
          case B111:
            turnAround();
            turn = 3;
            tempFoundVertex = false;
            orientRobot( caseVariable, turn );
            transmitSqData( yVal, xVal );
            stack.push(possibleBackNode);
            break;
          default:
            break;
        }
      }
      //      coordinateCalculation( orientation );

      //goStraight();
      //delay(100);
      foundVertex = tempFoundVertex;
      // Serial.println("found vertex ********************");
      //      readDistanceSensors();

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
    // // Serial.println("got here");
    avgLightMiddle = avgLightMiddle + analogRead( lightMiddlePort );
    avgLightLeft = avgLightLeft + analogRead( lightLeftPort );
    avgLightRight = avgLightRight + analogRead( lightRightPort );
    counterLight = counterLight + 1;
  }
  counterLight = 0;
  lightMiddleVal = avgLightMiddle / 3;
  //// Serial.println("hello");
  lightLeftVal = avgLightLeft / 3;
  lightRightVal = avgLightRight / 3;
  String left = "Left: ";
  // Serial.println(left);
  // Serial.println(lightLeftVal);
  String middle = ", Middle: ";
  // Serial.println(middle);
  // Serial.println(lightMiddleVal);
  String right = ", Right: ";
  // Serial.println(right);
  // Serial.println(lightRightVal);
  String lightOutput = left + lightLeftVal + middle + lightMiddleVal + right + lightRightVal;
  // Serial.println( lightOutput );

  middleIsWhite = (lightMiddleVal < UPPER_MIDDLE_BOUND_WHITE );
  middleIsBlack = (lightMiddleVal > LOWER_MIDDLE_BOUND_BLACK);
  rightIsBlack = (lightRightVal > LOWER_RIGHT_BOUND_BLACK);
  rightIsWhite = ( lightRightVal < UPPER_RIGHT_BOUND_WHITE );
  leftIsBlack = (lightLeftVal > LOWER_LEFT_BOUND_BLACK);
  leftIsWhite = (lightLeftVal < UPPER_LEFT_BOUND_WHITE );

  foundVertex = ( leftIsWhite && middleIsWhite && rightIsWhite );

  avgLightLeft = 0;
  avgLightRight = 0;
  avgLightMiddle = 0;
}




void readDistanceSensors() {
  int counter0 = 0;
  //int counter1 = 0;
  //int counter2 = 0;
  caseVariable = B000; // reset case variable
  avgLeftDistance = 0;
  avgMiddleDistance = 0;
  avgRightDistance = 0;
  /*
    while (counter0 < 5) {
     chooseChannel0();
     avgLeftDistance = avgLeftDistance + analogRead(muxOut);
     chooseChannel1();
     avgMiddleDistance = avgMiddleDistance + analogRead(muxOut);
     chooseChannel2();
     avgRightDistance = avgRightDistance + analogRead(muxOut);
     counter0 += 1;
    }*/
  //  chooseChannel1();
  //  while (counter1 < 5) {
  //    avgMiddleDistance = avgMiddleDistance + analogRead(muxOut);
  //    counter1 += 1;
  //  }
  //  chooseChannel2();
  //  while (counter2 < 5) {
  //    avgRightDistance = avgRightDistance + analogRead(muxOut);
  //    counter2 += 1;
  //  }
  chooseChannel0();
  LeftDistance = analogRead(muxOut); /*avgLeftDistance / 5;*/
  chooseChannel1();
  MiddleDistance = analogRead(muxOut); /*avgMiddleDistance / 5;*/
  chooseChannel2();
  RightDistance = analogRead(muxOut); /* avgRightDistance / 5; */
  byte a = B100;
  byte b = B010;
  byte c = B001;
  if ( LeftDistance > WALL_THRESHOLD && sawLeft ) {
    sawLeft = false;
    caseVariable = caseVariable | a; //set leftmost bit to 1
  }
  if ( MiddleDistance > (WALL_THRESHOLD) && sawMiddle ) {
    sawMiddle = false;
    caseVariable = caseVariable | b; //set middle bit to 1
  }
  if ( RightDistance > WALL_THRESHOLD && sawRight ) {
    sawRight = false;
    caseVariable = caseVariable | c;  //set rightmost bit to 1
  }
  if ( LeftDistance > WALL_THRESHOLD ) {
    sawLeft = true;
    for ( int ldLoop = 0; ldLoop < 15; ldLoop++ ) {
      chooseChannel0();
      LeftDistance = analogRead(muxOut);
      sawLeft &= LeftDistance > WALL_THRESHOLD;
    }
  }
  else sawLeft = false;
  if ( MiddleDistance > (WALL_THRESHOLD - 35) ) {
    sawMiddle = true;
    for ( int mdLoop = 0; mdLoop < 15; mdLoop++ ) {
      chooseChannel1();
      MiddleDistance = analogRead(muxOut);
      sawMiddle &= MiddleDistance > (WALL_THRESHOLD - 35);
    }
  }
  else sawMiddle = false;
  if ( RightDistance > WALL_THRESHOLD ) {
    sawRight = true;
    for ( int rdLoop = 0; rdLoop < 15; rdLoop++ ) {
      chooseChannel2();
      RightDistance = analogRead(muxOut);
      sawRight &= RightDistance > WALL_THRESHOLD;
    }
  }
  else sawRight = false;

  String l = "left dist ";
  String r = "  right dist ";
  String m = "  middle dist ";
  // Serial.println("hi");
  Serial.println(l + LeftDistance + m + MiddleDistance + r + RightDistance);
  //Serial.println(caseVariable);
  avgLeftDistance = 0;
  avgMiddleDistance = 0;
  avgRightDistance = 0;
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
  servoLeft.write( 95 );
  servoRight.write( 85 );
  delay( 50 );
  goStop();
  delay( 50 );
  readLightSensors();
  servoLeft.write( 98 );
  servoRight.write( 90 );
  int backup = 0;
  while ( middleIsBlack && (backup < 45 ) ) {
    readLightSensors();
    backup++;
  }
}

//used to be 90 and 80 just changed it to 90 and 60 as of 2:17pm 11/11
void goLeft( void )
{
  servoLeft.write( 95 );
  servoRight.write( 85 );
  delay( 50 );
  goStop();
  delay( 50 );
  readLightSensors();
  servoLeft.write( 90 );
  servoRight.write( 82 );
  int backup = 0;
  while ( middleIsBlack && (backup < 45 )) {
    readLightSensors();
    backup ++;
  }
}

void turnAround( void )
{
  servoLeft.write( 0 );
  servoRight.write( 0 );
  // Serial.println( "turning left" );
  delay( 100 );
  readLightSensors();
  while ( middleIsBlack ) {
    readLightSensors();
  }
  //        delay( 100 );
  //        while ( middleIsBlack ) {
  //          readLightSensors();
  //        }

}

void goStop( void )
{
  servoLeft.write( 90 );
  servoRight.write( 90 );
  //        delay(2000);
  // Serial.println( "going stopped" );
}


void correctLeft( void )
{
  servoLeft.write( 92 ); //stop one wheel
  servoRight.write( 85 );
  // Serial.println( "correcting left" );
}


void correctRight( void )
{
  servoLeft.write( 95 );
  servoRight.write( 88 ); //stop one wheel
  // Serial.println( "correcting right" );
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
    // Serial.println("not correcting");
  }
  //case 2, correct left, because I see left white.
  if ( leftIsWhite && rightIsBlack ) {
    correctLeft();
    // Serial.println("correcting left");
  }
  //case 3, correct right, because I see right white.
  if ( rightIsWhite && leftIsBlack ) {
    correctRight();
    // Serial.println("correcting right");
  }
}

void goStraight( void )
{
  servoLeft.write( 95 );
  servoRight.write( 85 );
  // Serial.println( "going straight" );
}
// END MOVEMENT DEFINITIONS


int tempADCSRA;
int tempTIMSK0;
int tempADMUX;
int tempDIDR0;
boolean r;
boolean start;


//boolean startSound( void )
//{
//  tempADCSRA = ADCSRA;
//  tempTIMSK0 = TIMSK0;
//  tempADMUX = ADMUX;
//  tempDIDR0 = DIDR0;
//  TIMSK0 = 0; // turn off timer0 for lower jitter
//  ADCSRA = 0xe5; // set the adc to free running mode
//  ADMUX = 0x45; // use adc4 yellow wire A4
//  DIDR0 = 0x01; // turn off the digital input for adc0
//  //while(1) { // reduces jitter
//  //counter = counter+1;
//  //cli();  // UDRE interrupt slows this way down on arduino1.0
//  for (int i = 0 ; i < 128 ; i += 2) { // save 256 samples
//    while (!(ADCSRA & 0x10)); // wait for adc to be ready
//    ADCSRA = 0xf5; // restart adc
//    byte m = ADCL; // fetch adc data
//    byte j = ADCH;
//    int k = (j << 8) | m; // form into an int
//    k -= 0x0200; // form into a signed int
//    k <<= 6; // form into a 16b signed int
//    fft_input[i] = k; // put real data into even bins
//    fft_input[i + 1] = 0; // set odd bins to 0
//  }
//  fft_window(); // window the data for better frequency response
//  fft_reorder(); // reorder the data before doing the fft
//  fft_run(); // process the data in the fft
//  fft_mag_log(); // take the output of the fft
//  sei();
//  Serial.println("start");
//  for (byte i = 0 ; i < FFT_N / 2 ; i++) {
//    Serial.println(fft_log_out[i]); // send out the data
//  }
//  //// Serial.println(fft_log_out[5]);
//  start = false;
//  average = average + fft_log_out[3];
//  String avg = "average ";
//  //// Serial.println(avg+average);
//  if (counter == 5) {
//    average = average / 5;
//
//    //// Serial.println(avg+average);
//    if (average > 120) {
//      Serial.println("****************************this is 6.6kHz");
//      //goStop();
//      //delay(5000);
//      start = true;
//    }
//    else {
//      start = false;
//    }
//    counter = 0;
//    average = 0;
//  }
//  counter += 1;
//  //// Serial.println(counter);
//  //}
//  ADCSRA = tempADCSRA;
//  TIMSK0 = tempTIMSK0;
//  ADMUX = tempADMUX;
//  DIDR0 = tempDIDR0;
//  //goStraight();
//  return start;
//}


boolean detectIR( void )
{
  tempADCSRA = ADCSRA;
  tempTIMSK0 = TIMSK0;
  tempADMUX = ADMUX;
  tempDIDR0 = DIDR0;
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x44; // use adc5 purple wire A5
  DIDR0 = 0x01; // turn off the digital input for adc0
  //while(1) { // reduces jitter
  //counter = counter+1;
  // cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 256 ; i += 2) { // save 256 samples
    while (!(ADCSRA & 0x10)); // wait for adc to be ready
    ADCSRA = 0xf5; // restart adc
    byte m = ADCL; // fetch adc data
    byte j = ADCH;
    int k = (j << 8) | m; // form into an int
    k -= 0x0200; // form into a signed int
    k <<= 6; // form into a 16b signed int
    fft_input[i] = k; // put real data into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();
  //// Serial.println("start");
  for (byte i = 0 ; i < FFT_N / 2 ; i++) {
    // // Serial.println(fft_log_out[i]); // send out the data
  }

  r = false;
  average = average + fft_log_out[42];
  String avg = "average ";
  //// Serial.println(avg+average);
  if (counter == 5) {
    average = average / 5;

    //// Serial.println(avg+average);
    if (average > 400) {
      // Serial.println("****************************this is 6kHz");
      //goStop();
      //delay(5000);
      r = true;
    }
    else {
      r = false;
    }
    counter = 0;
    average = 0;
  }
  counter += 1;
  //// Serial.println(counter);
  //}
  ADCSRA = tempADCSRA;
  TIMSK0 = tempTIMSK0;
  ADMUX = tempADMUX;
  DIDR0 = tempDIDR0;
  //goStraight();
  return r;
}


/*********** RADIO STUFF BELOW THIS LINE **********/

/*********** TO BE RUN IN setup() ***********/
//grey ground, green power
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
  // Serial.println( "reset" );
  transmit( 0xFF );
  //transmit( 0xFF );
  //transmit( 0xFF );
}


/* MAIN TRANSMIT FUNCTION, CALL ON TRAVERSING TO
    NEW SQUARE
*/
void transmitSqData( int xVal, int yVal )
{
  //    assignGlobalMaze( xVal, yVal ); //FOR SIMULATION ONLY !!!!!!!!!!
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
  //delay( 250 );
  transmit( firstByte );
  //delay( 250 );
  transmit( second );
  //delay( 250 );
  transmit( secondByte);
  //delay( 250 );
  transmit( coords );
  //delay( 250 );
  transmit( coordinates );
  //delay( 2000 );
}


void orientRobot( int wallState, int turn )
{
  switch ( wallState ) {
    case B001: //right wall only
      switch ( orientation ) {
        case north_up:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = true;
          break;
        case north_left:
          northWall = false;
          westWall = false;
          southWall = true;
          eastWall = false;
          break;
        case north_back:
          northWall = false;
          westWall = true;
          southWall = false;
          eastWall = false;
          break;
        case north_right:
          northWall = true;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;
      }
      if (turn == 1) {
        if ( orientation > 0 ) {
          orientation = orientation - 1;
        }
        else {
          orientation = 3;
        }
      }
      break;
    case B101: //left and right only
      switch ( orientation ) {
        case north_up:
          northWall = false;
          westWall = true;
          southWall = false;
          eastWall = true;
          break;
        case north_left:
          northWall = true;
          westWall = false;
          southWall = true;
          eastWall = false;
          break;
        case north_back:
          northWall = false;
          westWall = true;
          southWall = false;
          eastWall = true;
          break;
        case north_right:
          northWall = true;
          westWall = false;
          southWall = true;
          eastWall = false;
          break;
      }
      break;
    case B100: //left wall only
      switch ( orientation ) {
        case north_up:
          northWall = false;
          westWall = true;
          southWall = false;
          eastWall = false;
          break;
        case north_left:
          northWall = true;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;
        case north_back:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = true;
          break;
        case north_right:
          northWall = false;
          westWall = false;
          southWall = true;
          eastWall = false;
          break;
      }
      if (turn == 2) {
        if ( orientation < 3 ) {
          orientation = orientation + 1;
        }
        else {
          orientation = 0;
        }
      }
      break;
    case B110: //left and front wall
      switch ( orientation ) {
        case north_up:
          northWall = true;
          westWall = true;
          southWall = false;
          eastWall = false;
          break;

        case north_left:
          northWall = true;
          westWall = false;
          southWall = false;
          eastWall = true;
          break;

        case north_back:
          northWall = false;
          westWall = false;
          southWall = true;
          eastWall = true;
          break;

        case north_right:
          northWall = false;
          westWall = true;
          southWall = true;
          eastWall = false;
          break;
        default:
          break;
      }
      if ( orientation < 3 ) {
        orientation = orientation + 1;
      }
      else {
        orientation = 0;
      }
      break;
    case B011: //right and front wall
      switch ( orientation ) {
        case north_up:
          northWall = true;
          westWall = false;
          southWall = false;
          eastWall = true;
          break;

        case north_left:
          northWall = false;
          westWall = false;
          southWall = true;
          eastWall = true;
          break;

        case north_back:
          northWall = false;
          westWall = true;
          southWall = true;
          eastWall = false;
          break;

        case north_right:
          northWall = true;
          westWall = true;
          southWall = false;
          eastWall = false;
          break;
        default:
          break;
      }
      if ( orientation > 0 ) {
        orientation = orientation - 1;
      }
      else {
        orientation = 3;
      }
      break;
    case B010: //front wall only, default to turn left
      switch ( orientation ) {
        case north_up:
          northWall = true;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;

        case north_left:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = true;
          break;

        case north_back:
          northWall = false;
          westWall = false;
          southWall = true;
          eastWall = false;
          break;

        case north_right:
          northWall = false;
          westWall = true;
          southWall = false;
          eastWall = false;
          break;
        default:
          break;
      }
      if (turn == 1) {
        if ( turn > 0 ) {
          orientation = orientation - 1;
        }
        else {
          orientation = 3;
        }
      }
      else if (turn == 2) {
        if ( orientation < 3 ) {
          orientation = orientation + 1;
        }
        else {
          orientation = 0;
        }
      }
      break;
    case B000:
      switch (orientation) {
        case north_up:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;

        case north_left:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;

        case north_back:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;

        case north_right:
          northWall = false;
          westWall = false;
          southWall = false;
          eastWall = false;
          break;
        default:
          break;
      }
      if (turn == 1) {
        if ( turn > 0 ) {
          orientation = orientation - 1;
        }
        else {
          orientation = 3;
        }
      }
      else if (turn == 2) {
        if ( orientation < 3 ) {
          orientation = orientation + 1;
        }
        else {
          orientation = 0;
        }
      }
      break;
    case B111: //front, left, and right walls
      switch ( orientation ) {
        case north_up:
          northWall = true;
          westWall = true;
          southWall = false;
          eastWall = true;
          break;

        case north_left:
          northWall = true;
          westWall = false;
          southWall = true;
          eastWall = true;
          break;

        case north_back:
          northWall = false;
          westWall = true;
          southWall = true;
          eastWall = true;
          break;

        case north_right:
          northWall = true;
          westWall = true;
          southWall = true;
          eastWall = false;
          break;
        default:
          break;
      }
      if ( orientation > 1 ) {
        orientation = orientation - 2;
      }
      else {
        orientation = orientation + 2;
      }
      break;
    default:
      northWall = false;
      westWall = false;
      southWall = false;
      eastWall = false;
      break;
  }
}



//void coordinateCalculation( Orientation orientation ) {
//  switch ( orientation ) {
//    //case north_up:
//    case 0:
//      yVal -= 1;
//      break;
//    //case north_left:
//    case 1:
//      xVal += 1;
//      break;
//    //case north_back:
//    case 2:
//      yVal += 1;
//      break;
//    //case north_right:
//    case 3:
//      xVal -= 1;
//      break;
//    default:
//      break;
//  }
//}


/*
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

*/

/** USAGE FOR THE BELOW THREE PROGRAMS
    checkWalls checks for walls
      north/south/east/westWall are global variables
      please assign them values when checking in each square
    checkTreasure checks the value of treasure
      again, they're global variables, assign them
      values and this will byte encode them
    checkIr does the same thing for other robots
      again it's a global variable
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



//NOT USING STUFF BELOW THIS
//      if ( MiddleDistance > 175) {
//        if (LeftDistance > 200) {
//
//        }
//        else if (RightDistance > 175) {
//          goLeft();
//          //// Serial.println( "Right Wall" );
//          foundVertex = false;
//        }
//        //// Serial.println( "got right" );
//        //goStop();
//        //      delay( 100 );
//        else {
//          goLeft();
//          //// Serial.println( "Middle Wall" );
//          foundVertex = false;
//        }
//      }
//      else{

// foundVertex = false;
//      }

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
  //delay(1000);
  //}
}
