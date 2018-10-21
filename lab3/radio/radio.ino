/*
  Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 2 as published by the Free Software Foundation.
*/

/**
  Example for Getting Started with nRF24L01+ radios.

  This is an example of how to use the RF24 class.  Write this sketch to two
  different nodes.  Put one of the nodes into 'transmit' mode by connecting
  with the serial monitor and sending a 'T'.  The ping node sends the current
  time to the pong node, which responds by sending the value back.  The ping
  node can then see how long the whole cycle took.
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

/***
   DEFINING BIT MASKS TO USE
   4 bits - WALLS ( N, E, S, W )
   2 bits - TREASURE SHAPE ( NONE, TRIANGLE, CIRCLE, SQUARE )
   1 bits - TREASURE COLOR ( RED, BLUE )
   1 bits - ROBOT IS HERE ( YES, NO, FROM IR )
   1 bits - EXPLORED? ( YES, NO )
***/

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

/*** FINISHED DEFINING BIT MASKS **/
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
role_e role = role_pong_back;


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


String parsedCoordinates;
String parsedFirstByte;
String parsedSecondByte;

int printFlag; 

//CREATE MAZE
/**

      __0____1____2__
   2 | tTR       tSB |
     |    ___________|
   1 |       R       |
     |___________    |
   0 |      tCR      |
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
  { { true, true, false, true }, { true, true, false, false, true, false, false, true }, {false, true, true, false} },
  { { false, true, false, true }, { true, true, false, false, false, false, false, false, false, true }, { true, false, true, false } },
  { { true, false, false, true, false, true, false, true }, { true, true, false, false }, { true, true, true, false, false, false, true, false, true } }
};


void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r", role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

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

  radio.printDetails();
}

void loop(void)
{
  while ( role == role_pong_back ) {
    receive();
    roleChange();
  } 
  for ( int yVal = 0; yVal < 3; yVal++ ) {
    for ( int xVal = 0; xVal < 3; xVal++ ) {
      
      assignGlobalMaze( xVal, yVal );
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
  }
  byte ended = 0xFF;
  transmit( ended );
  delay( 250 );
}


void assignGlobalMaze ( int xVal, int yVal ) {
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


// vim:cin:ai:sts=2 sw=2 ft=cpp


void roleChange( void ) {
  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == role_pong_back )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

      // Become the primary transmitter (ping out)
      role = role_ping_out;
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1, pipes[1]);
    }
    else if ( c == 'R' && role == role_ping_out )
    {
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");

      // Become the primary receiver (pong back)
      role = role_pong_back;
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1, pipes[0]);
    }
  }
}


byte checkWalls( byte firstByte ) {
  if ( northWall ) firstByte |= WALL_NORTH_PRESENT;
  if ( southWall ) firstByte |= WALL_SOUTH_PRESENT;
  if ( eastWall ) firstByte |= WALL_EAST_PRESENT;
  if ( westWall ) firstByte |= WALL_WEST_PRESENT;
  return firstByte;
}


byte checkTreasure( byte firstByte ) {
  if ( treasureTriangle ) firstByte |= TREASURE_TRIANGLE;
  if ( treasureCircle ) firstByte |= TREASURE_CIRCLE;
  if ( treasureSquare ) firstByte |= TREASURE_SQUARE;
  if ( treasureRed ) firstByte |= TREASURE_RED;
  if ( treasureBlue ) firstByte |= TREASURE_BLUE;
  return firstByte;
}


byte checkIr( byte firstByte ) {
  if ( robotPresent ) {
    firstByte |= ROBOT_PRESENT; 
    }
  else if ( !robotPresent ) firstByte &= ROBOT_NOT_PRESENT;
  return firstByte;
}


void transmit( byte payload ) {
  // Ping out role.  Repeatedly send the current time
  //

  if ( role == role_ping_out )
  {
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
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
  }
}

void receive( void ) {
  //
  // Pong back role.  Receive each packet, dump it out, and send it back
  //
  if ( role == role_pong_back )
  {
    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned char rxPayload;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &rxPayload, sizeof(unsigned char) );
        
        // Spew it
        //printf("Got payload %x...", rxPayload);
        if ( rxPayload == 0xFF ) {
          printf( "I have terminated the maze \r\n" );
        }
        else if ( rxPayload == 0x1b ) {
          printf( "\r\nThis is the first byte " );
          printFlag = 1;
        }
        else if ( rxPayload == 0x2b ) {
          printf( "This is the second byte " );
          printFlag = 2;
        }
        else if ( rxPayload == 0xcc ) {
          printf( "These are the coordinates " );
          printFlag = 3;
        }
        else {
          printf("%x \n\r", rxPayload);
          switch ( printFlag ):
          case 3: 
          
        }
        // Delay just a little bit to let the other unit
        // make the transition to receiver
        delay(20);

      }

      // First, stop listening so we can talk
      radio.stopListening();

      // Send the final one back.
      radio.write( &rxPayload, sizeof(unsigned char) );
      

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }
  }
}

String readCoordinates( byte coordinates ) {
  String output = "";
  int xVal = coordinates >> 4;
  int yVal = coordinates & 0x0F;
  output = xVal + "," + yVal;
  return output
}


String readFirstByte( byte firstByte ) {
  String output = "";
  //if ( firstByte >> 7 ) 
  return output;
}
