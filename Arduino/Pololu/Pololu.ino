#include <Pololu3piPlus32U4.h>
#include "my_robot.h"
using namespace Pololu3piPlus32U4;


MyRobot robot;
void led(){
  for (int i = 1; i <= 5; i++) {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(50);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
  delay(50);
  }                    // wait for a second
}

void setup() {
    Serial.begin(9600);
    delay(15);
    pinMode(LED_BUILTIN, OUTPUT);
}
//test the movements of the robot
void loop() {
  //create speed outputs in both positive and negative directions
  int speed = 300;
  int duration = 3000;
  int distance = 1;
  
  //move forward
    robot.forward(distance, speed);
    led();
    delay(3000);
  //spin right
    robot.spin_right(duration, speed);
    led();
    delay(3000);
  //spin left
    robot.spin_left(duration, speed);
    led();
    delay(3000);
  //move backward
    robot.backward(distance, speed);
    led();
    delay(3000);
  //move forward and right
    robot.turn_right(duration, speed);
    led();
    delay(3000);
  //move forward and right
    robot.turn_left(duration, speed);
    led();
    delay(3000);
  //stop robot motion
    robot.halt();
    led();
    led();
    led();
    delay(1000);
}



