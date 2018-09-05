/*
  Blink External

  Turns an external LED on for one second, then off for one second, repeatedly.

*/

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(14, OUTPUT); // attached LED to pin 14
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(14, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(14, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
