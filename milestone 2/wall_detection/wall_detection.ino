void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
  pinMode(6, OUTPUT); // red LED
  pinMode(7, OUTPUT); //white LED
  pinMode(5, OUTPUT); //green LED
}

void loop() {
  // put your main code here, to run repeatedly:
  int LeftDistance = analogRead(A3);
  int MiddleDistance = analogRead(A5);
  int RightDistance = analogRead(A4);
  
  String left = "left: ";
  String middle = "     middle:";
  String right = "        right: ";
  Serial.println( left + LeftDistance + middle + MiddleDistance + right + RightDistance );
  //green
  if(LeftDistance < 200){
    digitalWrite(5, HIGH);
  }
  else{
    digitalWrite(5, LOW);
  }
  //red
  if(RightDistance < 200){
    digitalWrite(6, HIGH);
  }
  else{
    digitalWrite(6, LOW);
  }
  //white
  if(MiddleDistance < 200){
    digitalWrite(7, HIGH);
  }
  else{
    digitalWrite(7, LOW);
  }
}
