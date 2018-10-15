#include <Servo.h>

int LEDOut= 3;
int potentiometerIn = A1;
int readVal= 0;
Servo servoOne;
Servo servoTwo;

// the setup function runs once when you press reset or power the board
void setup() {
  servoOne.attach(9);
  servoTwo.attach(10);
 
}

//// the loop function runs over and over again forever
//void loop() {
//  servoOne.write(140); 
//  servoTwo.write(40);
//}

//tries to do a right turn
void rightTurn() {
  servoOne.write(180); //something close to 180
  servoTwo.write(175); //something close to 90
  delay(750);
}

void straight(){
  servoOne.write(180); //something close to 180
  servoTwo.write(0); //something close to 90
  delay(2000);
}

//square
void loop() {
//  straight();
//  rightTurn();
    servoOne.write(90); 
    servoTwo.write(90);
}
