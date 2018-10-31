void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT); 
  pinMode(9, INPUT); 
  //00 means not red or blue
  //01 means red
  //10 means blue
}

int MSB = 0; //leftmost
int LSB = 0; //rightmost
void loop() {
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
