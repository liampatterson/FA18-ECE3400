#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21 /*TODO: write this in hex (eg. 0xAB) */


int reg11;
int reg0c;
int reg40;
int reg3e;
int reg12;
int reg42;

///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // TODO: READ KEY REGISTERS
  //read_key_registers();  //set the global registers
  
  delay(100);
  
  // TODO: WRITE KEY REGISTERS
  //there should be 8 of them
  Serial.println("got here");
  Serial.println(OV7670_write_register( 0x11, 0x80 )); //Use external clock as internal clock -- reg 11  set to 80--set to default
  Serial.println(OV7670_write_register( 0x0C, 0x08 )); //Enable scaling -- reg 0c
  Serial.println(OV7670_write_register( 0x40, 0x20 )); //reg 40 Change resolution
  Serial.println(OV7670_write_register( 0x12, 0x0C )); //change resolution
  Serial.println(OV7670_write_register( 0x12, 0x80 ));//color test reg 12 AND reset all registers by enabling last bit
  Serial.println(OV7670_write_register( 0x12, 0x02 ));
  Serial.println(OV7670_write_register( 0x42, 0x08 ));//more color test reg 42
  //read_key_registers();
  set_color_matrix();
    // put your setup code here, to run once:
  pinMode(8, INPUT); 
  pinMode(9, INPUT); 
  //00 means not red or blue
  //01 means red
  //10 means blue
}

int MSB = 0; //leftmost
int LSB = 0; //rightmost


void loop(){
    // put your main code here, to run repeatedly:
  
  MSB = digitalRead(8);
  LSB = digitalRead(9);
  if(MSB == LOW && LSB == LOW){ //nothing
    Serial.println("no color");
  }
  else if(MSB == LOW && LSB == HIGH){
    Serial.println("RED");
  }
  else if(MSB == HIGH && LSB == LOW){
    Serial.println("BLUE");
  }
}

///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  reg11 = read_register_value(17); //external clock
  reg0c = read_register_value(12); //enable scaling
  reg40 = read_register_value(64); //change res 
  reg3e = read_register_value(62); //change res
  reg12 = read_register_value(18); //change res
  reg42 = read_register_value(66); //change res
}


byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
  while(Wire.available()<1);
  data = Wire.read();
  return data;
}


String OV7670_write(int start, const byte *pData, int size){
    Serial.println("writing");
    int n,error;
    Wire.beginTransmission(OV7670_I2C_ADDRESS);
    n = Wire.write(start);
    if(n != 1){
      return "I2C ERROR WRITING START ADDRESS";   
    }
    n = Wire.write(pData, size);
    if(n != size){
      return "I2C ERROR WRITING DATA";
    }
    error = Wire.endTransmission(true);
    if(error != 0){
      return String(error);
    }
    return "no errors :)";
 }
 

String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }
 

void set_color_matrix(){
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
