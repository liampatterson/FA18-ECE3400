//lightSensor sample code for Sparkfun QRE1113 (Analog) breakout
//uses pin A0 for connectivity, prints value received.

int linePin = A0;
int lowerBoundWhite = 0;
int upperBoundWhite = 0;
int lowerBoundBlack = 0;
int upperBoundBlack = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead( linePin );
  if( val < upperBoundWhite && val > lowerBoundWhite){
    //go straight
  }
  else{ //
  Serial.println( val );
}
