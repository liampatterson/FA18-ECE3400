void setup() {
  // put your setup code here, to run once:
Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  readDistanceSensors();
}

int avgMiddleDistance;
int MiddleDistance;
void readDistanceSensors(){
    int counter = 0;
    avgMiddleDistance = 0;
    while (counter < 5) {
    
        avgMiddleDistance = avgMiddleDistance + analogRead(A0);
    
        counter+=1;
    }
    MiddleDistance = avgMiddleDistance / 5;
    if(MiddleDistance>130){    
      Serial.println("this is a wall *************");
    } 
    String m = "  middle dist ";
     Serial.println(m+MiddleDistance);
}
