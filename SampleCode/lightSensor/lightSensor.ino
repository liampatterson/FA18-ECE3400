//lightSensor sample code for Sparkfun QRE1113 (Analog) breakout
//uses pin A0 for connectivity, prints value received.

int linePin = A0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  int val = analogRead( linePin );
  Serial.println( val );
}
