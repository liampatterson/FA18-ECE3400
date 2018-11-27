#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21 /*TODO: write this in hex (eg. 0xAB) */


int reg11;
int reg0C;
int reg40;
int reg3e;
int reg12;
int reg42;
int reg14;

///////// Main Program //////////////
void setup() {
  String output;
  Wire.begin();
  Serial.begin(9600);
  Serial.println( "setup starting" );

  // TODO: READ KEY REGISTERS
  read_key_registers();  //set the global registers

  delay(100);

  // TODO: WRITE KEY REGISTERS
  //there should be 8 of them

  Serial.println("starting write");
  Serial.println(OV7670_write_register( 0x12, 0x80 ));
  //delay
  delay(100);
  Serial.println(OV7670_write_register( 0x12, 0x0C ));  //c should disable test, E should enable everything
  Serial.println(OV7670_write_register( 0x11, 0xC0 ));
  Serial.println(OV7670_write_register( 0x0C, 0x08 ));
  Serial.println(OV7670_write_register( 0x40, 0xD0 )); //confirmed good except output seems whack
  Serial.println(OV7670_write_register( 0x42, 0x00 ));//more color test reg 42, 0 will turn off, 08 will turn on
  Serial.println(OV7670_write_register( 0x14, 0x0B ));

  // to turn color test on:
  // replace "Serial.println(OV7670_write_register( 0x12, 0x0C ));" with  Serial.println(OV7670_write_register( 0x12, 0x0E ));
  // replace "Serial.println(OV7670_write_register( 0x42, 0x00 ));" with Serial.println(OV7670_write_register( 0x42, 0x08 ));


  read_key_registers();
  set_color_matrix();
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  //00 means not red or blue
  //01 means red
  //10 means blue
  Serial.println( "setup finished." );
}

//int MSB = 0; //leftmost
//int LSB = 0; //rightmost
//int B = 0;
//
//
//void loop(){
//    // put your main code here, to run repeatedly:
//
//  MSB = digitalRead(8);
//  B = digitalRead(9);
//  LSB = digitalRead(10);
//  if(MSB == LOW && B == LOW && LSB == LOW){ //nothing
//    Serial.println("no color");
//  }
//  else if(MSB == LOW && B == LOW && LSB == HIGH){
//    Serial.println("BLUE TRIANGLE");
//  }
//  else if(MSB == LOW && B == HIGH && LSB == LOW){
//    Serial.println("BLUE DIAMOND");
//  }
//  else if(MSB == LOW && B == HIGH && LSB == HIGH){
//    Serial.println("BLUE SQUARE");
//  }
//  else if(MSB == HIGH && B == LOW && LSB == LOW){
//    Serial.println("RED TRIANGLE");
//  }
//  else if(MSB == HIGH && B == LOW && LSB == HIGH){
//    Serial.println("RED DIAMOND");
//  }
//  else if(MSB == HIGH && B == HIGH && LSB == LOW){
//    Serial.println("RED SQUARE");
//  }
//  //Serial.println( "loop" );
//}

int MSB = 0; //leftmost
int LSB = 0; //rightmost
int B = 0;
int redSquare = 0;
int redTriangle = 0;
int redDiamond = 0;
int blueSquare = 0;
int blueTriangle = 0;
int blueDiamond = 0;

void loop() {
// put your main code here, to run repeatedly:
  MSB = digitalRead(8);
  B = digitalRead(9);
  LSB = digitalRead(10);

  if (MSB == HIGH && B == LOW && LSB == LOW) {
    redTriangle++;
    redSquare = 0;
    redDiamond = 0;
    blueSquare = 0;
    blueTriangle = 0;
    blueDiamond = 0;
  }
  else if (MSB == HIGH && B == HIGH && LSB == LOW) {
    redSquare++;
    redTriangle = 0;
    redDiamond = 0;
    blueSquare = 0;
    blueTriangle = 0;
    blueDiamond = 0;
   }
  else if (MSB == HIGH && B == LOW && LSB == HIGH) {
    redDiamond++;
    redSquare = 0;
    redTriangle = 0;
    blueSquare = 0;
    blueTriangle = 0;
    blueDiamond = 0;
  }
  else if (MSB == LOW && B == LOW && LSB == HIGH) {
    blueTriangle++;
    redSquare = 0;
    redTriangle = 0;
    redDiamond = 0;
    blueSquare = 0;
    blueDiamond = 0;
  }
  else if (MSB == LOW && B == HIGH && LSB == HIGH) {
    blueSquare++;
    redSquare = 0;
    redTriangle = 0;
    redDiamond = 0;
    blueTriangle = 0;
    blueDiamond = 0;
  }
  else if (MSB == LOW && B == HIGH && LSB == LOW) {
    blueDiamond++;
    redSquare = 0;
    redTriangle = 0;
    redDiamond = 0;
    blueSquare = 0;
    blueTriangle = 0;
  }
  else { //nothing
    Serial.println("No Treasure");
    redSquare = 0;
    redTriangle = 0;
    redDiamond = 0;
    blueSquare = 0;
    blueTriangle = 0;
    blueDiamond = 0;
  }

  if (redSquare > 10) {
    Serial.println("Red Square");
  }
  else if (redTriangle > 10) {
    Serial.println("Red Triangle");
  }
  else if (redDiamond > 10) {
    Serial.println("Red Diamond");
  }
  else if (blueSquare > 10) {
    Serial.println("Blue Square");
  }
  else if (blueTriangle > 10) {
    Serial.println("Blue Triangle");
  }
  else if (blueDiamond > 10) {
    Serial.println("Blue Diamond");
  }
  else {
    Serial.println("No treasure");
  }
}

///////// Function Definition //////////////
void read_key_registers() {
  /*TODO: DEFINE THIS FUNCTION*/
  reg11 = read_register_value(0x11); //external clock
  Serial.println( "reg11" );
  Serial.println( reg11 );
  reg12 = read_register_value(0x12); //enable scaling
  Serial.println( "reg12" );
  Serial.println( reg12 );
  reg40 = read_register_value(0x40); //change res
  Serial.println( "reg40" );
  Serial.println( reg40 );
  //  reg3e = read_register_value(62); //change res
  //  Serial.println( reg3e );
  //  reg12 = read_register_value(18); //change res
  //  Serial.println( reg12 );
  reg42 = read_register_value(0x42); //change res
  Serial.println( "reg42" );
  Serial.println( reg42 );
  reg0C = read_register_value(0x0C); //change res
  Serial.println( "reg0c" );
  Serial.println( reg0C );
  reg14 = read_register_value(0x14); //change res
  Serial.println( "reg14" );
  Serial.println( reg14 );
  Serial.println( "finished read" );

}


byte read_register_value(int register_address) {
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Serial.println( "begining read_reg_val" );
  //Serial.println( register_address );
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS, 1);
  while (Wire.available() < 1);
  data = Wire.read();
  return data;
}


String OV7670_write(int start, const byte *pData, int size) {
  int n, error;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Serial.println( "starting transmission" );
  n = Wire.write(start);
  if (n != 1) {
    return "I2C ERROR WRITING START ADDRESS";
  }
  n = Wire.write(pData, size);
  if (n != size) {
    return "I2C ERROR WRITING DATA";
  }
  error = Wire.endTransmission(true);
  if (error != 0) {
    return String(error);
  }
  return "no errors :)";
}


String OV7670_write_register(int reg_address, byte data) {
  return OV7670_write(reg_address, &data, 1);
}


void set_color_matrix() {
  OV7670_write_register(0x4f, 0x80);
  OV7670_write_register(0x50, 0x80);
  OV7670_write_register(0x51, 0x00);
  OV7670_write_register(0x52, 0x22);
  OV7670_write_register(0x53, 0x5e);
  OV7670_write_register(0x54, 0x80);
  OV7670_write_register(0x56, 0x40);
  OV7670_write_register(0x58, 0x9e);
  OV7670_write_register(0x59, 0x88);
  OV7670_write_register(0x5a, 0x88);
  OV7670_write_register(0x5b, 0x44);
  OV7670_write_register(0x5c, 0x67);
  OV7670_write_register(0x5d, 0x49);
  OV7670_write_register(0x5e, 0x0e);
  OV7670_write_register(0x69, 0x00);
  OV7670_write_register(0x6a, 0x40);
  OV7670_write_register(0x6b, 0x0a);
  OV7670_write_register(0x6c, 0x0a);
  OV7670_write_register(0x6d, 0x55);
  OV7670_write_register(0x6e, 0x11);
  OV7670_write_register(0x6f, 0x9f);
  OV7670_write_register(0xb0, 0x84);
}
