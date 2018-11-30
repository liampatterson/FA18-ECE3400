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

/*** FINISHED DEFINING BIT MASKS **/
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9, 10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
uint64_t pipes[2] = { 0x000000002ALL, 0x000000002BLL };
//2(3D + N) + X
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

uint64_t pipesToHit[80] = { 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
                              81, 81, 81, 81, 81, 81, 81, 81, 81, 81 };

int currHitIndex = 0;

bool runOnce = false;

void setup(void)
{
  //
  // Print preamble
  //
  role = role_ping_out;
    
  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15, 3);
  radio.setAutoAck(true);
  // set the channel
  radio.setChannel(0x50);
  // set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm.
  radio.setPALevel(RF24_PA_MAX);
  radioSetup();
  Serial.begin(115200);
  Serial.println( "initializing..." );
}


void loop(void)
{
  role = role_ping_out;
  Serial.println( "Sniffing..." );
  runOnce = false;
  //currHitIndex = 0;
  for ( int wild = 0xC4; wild < 0xC5; wild+=2 ) {
    for ( uint64_t iterate = 0LL; iterate < 80; iterate++ ) {
          pipes[0] = iterate;
          pipes[1] = iterate+1;
          radioSetup();
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          radio.setPayloadSize(8);
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          radio.setPayloadSize(2);
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          }
        for ( uint64_t iterate = 80LL; iterate > 0; iterate-- ) {
          pipes[0] = iterate;
          pipes[1] = iterate-1;
          radioSetup();
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          radio.setPayloadSize(8);
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          radio.setPayloadSize(2);
          radio.setDataRate(RF24_2MBPS);
          transmit( (byte) wild );
          radio.setDataRate(RF24_250KBPS);
          transmit( (byte) wild );
          }
          //Serial.println( wild );
    }
    runOnce = true;
    Serial.println( currHitIndex );
    for ( int wild = 0; wild < 0xFF; wild++ ) {
      for ( int index = 0; index < currHitIndex; index+=2 ) {
        pipes[0] = pipesToHit[index];
        pipes[1] = pipesToHit[index+1];
        if ( pipes[0] == 9 | pipes[0] == 10 | pipes[1] == 9 | pipes[1] == 10 | pipes[0] == 12 | pipes[1] == 12 ) {
            Serial.println( "not gonna block ourselves" );   
        }
        else {
            radioSetup();
            radio.setDataRate(RF24_2MBPS);
            transmit( (byte) wild );
            radio.setDataRate(RF24_250KBPS);
            transmit( (byte) wild );
            radio.setPayloadSize(8);
            radio.setDataRate(RF24_2MBPS);
            transmit( (byte) wild );
            radio.setDataRate(RF24_250KBPS);
            transmit( (byte) wild );
            radio.setPayloadSize(2);
            radio.setDataRate(RF24_2MBPS);
            transmit( (byte) wild );
            radio.setDataRate(RF24_250KBPS);
            transmit( (byte) wild );
            Serial.println( (int) pipes[0] );
            Serial.println( (int) pipes[1] );
            Serial.println( wild );
            Serial.println( "============" );
        }
      }
    }
}


void radioSetup( void )
{
    printf_begin();
  //printf("\n\rRF24/examples/GettingStarted/\n\r");
  //printf("ROLE: %s\n\r", role_friendly_name[role]);
  //printf("*** PRESS 'T' to begin transmitting to the other node\n\r");

  //
  // Setup and configure rf radio
  //

  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(32);

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
  //Serial.println( "reset" );
}



void transmit( byte payload ) {
  // Ping out role.  Repeatedly send the current time
  //

  if ( role == role_ping_out )
  {
    // First, stop listening so we can talk.
    radio.stopListening();
    
    //printf( "Now sending %x...", payload );
    bool ok = radio.write( &payload, sizeof(unsigned char) );
    if ( ok & !runOnce ){
      //if ( pipes[0] == 9 | pipes[0] == 10 | pipes[1] == 9 | pipes[1] == 10 | pipes[0] == 12 | pipes[1] == 12 ) {
        //Serial.println( "not added" );
      //}
      //else {
        pipesToHit[ currHitIndex ] = pipes[ 0 ];
        pipesToHit[ currHitIndex + 1 ] = pipes[ 1 ];
        currHitIndex += 2;
        if ( currHitIndex > 81 ) 
            currHitIndex = 0;
        Serial.println( "added new" );
        Serial.println( (int) pipes[0] );
        Serial.println( currHitIndex );
      //}
      //Serial.println(" ================== ");
    }
    //successful
      //
    //else
      //printf( "failed.\n\r" );

    // Now, continue listening
//    radio.startListening();
//
//    // Wait here until we get a response, or timeout (250ms)
//    unsigned long started_waiting_at = millis();
//    bool timeout = false;
//    while ( ! radio.available() && ! timeout )
//      if (millis() - started_waiting_at > 2 )
//        timeout = true;

    // Describe the results
//    if ( timeout )
//    {
//     // printf("Failed, response timed out.\n\r");
//    }
//    else
//    {
//      // Grab the response, compare, and send to debugging spew
//      unsigned char rxPayload;
//      radio.read( &rxPayload, sizeof(unsigned char) );
//      if ( rxPayload == payload ) {
//        // Spew it
//        printf("Received correct data %x, RT delay: %lu\n\r", rxPayload, millis() - started_waiting_at);
//      }
//      else {
//        printf("That's not what I sent, received %x, RT delay: %lu\n\r", rxPayload, millis() - started_waiting_at);
//      }
//    }

    // Try again 1s later
    //delay(1);
  }
}


//void receive( void ) {
//  //
//  // Pong back role.  Receive each packet, dump it out, and send it back
//  //
//  if ( role == role_pong_back )
//  {
//    // if there is data ready
//    if ( radio.available() )
//    {
//      // Dump the payloads until we've gotten everything
//      unsigned char rxPayload;
//      bool done = false;
//      while (!done)
//      {
//        // Fetch the payload, and see if this was the last one.
//        done = radio.read( &rxPayload, sizeof(unsigned char) );
//        
//        // Spew it
//        //printf("Got payload %x...", rxPayload);
//        if ( rxPayload == 0xFF ) {
//          printf( "reset \r\n" );
//        }
//        else if ( rxPayload == 0x1b ) {
//          //printf( "\r\nThis is the first byte " );
//          printFlag = 1;
//        }
//        else if ( rxPayload == 0x2b ) {
//          //printf( "This is the second byte " );
//          printFlag = 2;
//        }
//        else if ( rxPayload == 0xcc ) {
//          //printf( "These are the coordinates " );
//          printFlag = 3;
//        }
//        else {
//          //printf("%x \n\r", rxPayload);
//          switch ( printFlag ) {
//            case 3: 
//              coordinateString = readCoordinates( rxPayload );
//              Serial.println( coordinateString + firstString );
//              break;
//            case 2:
//              //Serial.println( "2" );
//              break;
//            case 1:
//              firstString = readFirstByte( rxPayload );
//              break;
//            default:
//              break;
//            
//          
//          }
//        }
//        // Delay just a little bit to let the other unit
//        // make the transition to receiver
//        delay(20);
//
//      }
//
//      // First, stop listening so we can talk
//      radio.stopListening();
//
//      // Send the final one back.
//      radio.write( &rxPayload, sizeof(unsigned char) );
//      
//
//      // Now, resume listening so we catch the next packets.
//      radio.startListening();
//    }
//  }
//}
