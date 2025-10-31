#include <Pololu3piPlus32U4.h>
#include <Servo.h>
#include "sonar.h"
#include "Pcontroller.h"
#include "printOLED.h"
using namespace Pololu3piPlus32U4;


//Odometry Parameters
#define diaL 3.2
#define diaR  3.2
#define nL 12
#define nR 12
#define w 9.6
#define gearRatio 75

//Update kp and kd based on your testing
#define minOutput -400
#define maxOutput 400
#define kp 10
#define base_speed 50

Motors motors;
Servo servo;
PrintOLED oled;
Sonar sonar(4);

Pcontroller Pcontroller (kp, minOutput, maxOutput);

const double distFromWall=10.0; // Goal distance from wall (cm)

double wallDist;

void setup() {
  Serial.begin(9600);
  servo.attach(5);
  delay(40);
  //Move Sonar to desired direction using Servo
  servo.write(180);

  // while (Serial.available() == 0) {
  //   Serial.println("Waiting..."); //wait for us to be ready to start program with any user input
  //   delay(1000);
  //}
}

void loop() {

  //DO NOTE DELETE CODE AFTER EACH TASK, COMMENT OUT INSTEAD
  wallDist = sonar.readDist();
  Serial.println("Wall distance: ");
  Serial.println(wallDist);
  //UNCOMMENT AFTER IMPLEMENTING Pcontroller
  double Pout = Pcontroller.update(wallDist, distFromWall); //uncomment if using Pcontroller 

  //(LAB 4 - TASK 3.1) IMPLEMENT PCONTROLLER 
  
  /*FIRST GO TO Pcontroller.h AND ADD PRIVATE VARIABLES NEEDED.
    THEN GO TO Pcontroller.cpp AND COMPLETE THE update FUNCTION.
    ONCE YOU IMPLEMENT update, UNCOMMENT CODE ABOVE TO USE CONTROLLER.*/

  //(LAB 4 - TASK 3.2) PCONTROLLER WALL FOLLOWING

  /*NOW THAT YOU HAVE IMPLEMENTED PCONTROLLER, TAKE THE OUTPUT FROM Pout
  AND SET THE MOTOR SPEEDS. CHANGE THE KP AND CLAMPING VALUES AT THE TOP
  TO TEST (B-D).
  Hint: Also use baseSpeed when setting motor speeds*/


  //Also print outputs to serial monitor for testing purposes



   // Set motor speeds
    int leftSpeed = base_speed + Pout;
    int rightSpeed = base_speed - Pout;

    // Clamp motor speeds to valid range (-100 to 100)
    leftSpeed = constrain(leftSpeed, -100, 100);
    rightSpeed = constrain(rightSpeed, -100, 100);

    motors.setSpeeds(leftSpeed, rightSpeed);
    
    // Print debugging info
    Serial.println("Pout: ");
    Serial.println(Pout);
    Serial.println(" Left Speed: ");
    Serial.println(leftSpeed);
    Serial.println(" Right Speed: ");
    Serial.println(rightSpeed);
    oled.print_kp_dist(kp, wallDist); //print to oled

    Serial.println("Wall distance: "); //print tot serial
    Serial.println(wallDist);
    Serial.println("Kp value: ");
    Serial.println(kp);
    delay(100); // Small delay for stability

}
