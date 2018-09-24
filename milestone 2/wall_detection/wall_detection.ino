void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  pinMode(7, OUTPUT); // red LED
  pinMode(6, OUTPUT); //white LED
  pinMode(5, OUTPUT); //green LED
}

void loop() {
  // put your main code here, to run repeatedly:
  int LeftDistance = analogRead(A3);
  int RightDistance = analogRead(A4);
  String l = "left: ";
  String r = "        right: ";
  Serial.println( l + LeftDistance + r + RightDistance );
  if(LeftDistance > 200){
    digitalWrite(5, HIGH);
  }
  else{
    digitalWrite(5, LOW);
  }
  if(RightDistance > 200){
    digitalWrite(6, HIGH);
  }
  else{
    digitalWrite(6, LOW);
  }
}

// look at two LEDs like Binary
// Left is White (blinks green)
// Right is Red 
// White = bit 1 (MSB)
// Red = bit 0 (LSB)
// 00 means no walls anywhere
// 01 means left wall detected
// 10 means right wall detected
// 11 means front wall detected
