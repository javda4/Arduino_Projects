#include <Pololu3piPlus32U4.h>
#include "printOLED.h"
#include "odometry.h" //If using odometry, import odometry.h and odometry.cpp
#include "PIDcontroller.h" //Import your PIDcontroller.h and PIDcontroller.cpp from last lab then uncomment
using namespace Pololu3piPlus32U4;

Motors motors;
Encoders encoders;
PrintOLED oled;
//Odometry Parameters
#define diaL 3.2
#define diaR  3.2
#define nL 12
#define nR 12
#define w 9.6
#define gearRatio 75
#define DEAD_RECKONING false

//Update kp, kd, and ki based on your testing (First PIDcontroller for angle)
#define minOutputAng -100
#define maxOutputAng 100
#define kpAng 10//Tune Kp here
#define kdAng 0.05 //Tune Kd here
#define kiAng 5.5 //Tune Ki here
#define clamp_iAng 10 //Tune ki integral clamp here
#define base_speedAng 50

//Update kp, kd, and ki based on your testing (Second PIDcontroller for velocity) (Task 2.3)
#define minOutputVel -100
#define maxOutputVel 100
#define kpVel 10 //Tune Kp here
#define kdVel 0.05 //Tune Kd here
#define kiVel 5.5 //Tune Ki here
#define clamp_iVel 10 //Tune ki integral clamp here
#define base_speedVel 50

Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, DEAD_RECKONING); //Uncomment if using odometry class
PIDcontroller pidAng(kpAng, kiAng, kdAng, minOutputAng, maxOutputAng, clamp_iAng); //Uncomment after you import PIDController
PIDcontroller pidVel(kpVel, kiVel, kdVel, minOutputVel, maxOutputVel, clamp_iVel);

//Feel free to use this in your PD/PID controller for target values
// Given goals in cm and radians
const float goal_x = 1;
const float goal_y = 1;
const float goal_theta = 1.57; // Must put in radians

//odometry
int16_t deltaL=0, deltaR=0;
int16_t encCountsLeft = 0, encCountsRight = 0;
float x, y, theta;

//Lab 7
//Note: Here are some suggested variables to use for your code.
double PIDout_theta, PIDout_distance; //Output variables for your controllers
double angle_to_goal, actual_angle; //Keeping track of angle
double dist_to_goal = 0.0; //Keeping track of robot's distance to goal location

void setup() {
  Serial.begin(9600);
}

void loop() {

  //Use this code if you are using odometry. Comment out if you are not.
  //If using, consider turning this into its own function for repeated use.
  // Read data from encoders
  deltaL = encoders.getCountsAndResetLeft();
  deltaR = encoders.getCountsAndResetRight();

  // Increment total encoder cound
  encCountsLeft += deltaL;
  encCountsRight += deltaR;  

  odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta); //calculate robot's position


  //Lab 7
  //Note: To help with testing, print the theta and PID outputs to serial monitor.

  /*TASK 2.1
  Move your PIDController.h and PIDController.cpp files here to use for the following tasks.
  Also move your odometry.h and odometry.cpp if you decide to use it for 
  measuring the angle of your robot.
  
  Utilize your PIDController to go to a specific location.
  
  Hint: Utilize these functions to find your thetas
  angle_to_goal = atan2(?, ?);
  //atan2(sin(x),cos(x))=x on [-π, π) and not on [0,2π) 
  //=> we do this to make sure the range of actual_angle and goal_to_angle is the same
  actual_angle = atan2(?, ?);
  
  Write your code below and comment out when moving to the next task.*/
  // Compute angles for PID control
    angle_to_goal = atan2(goal_y - y, goal_x - x);
    actual_angle = atan2(sin(theta), cos(theta));
    dist_to_goal = sqrt(pow(goal_x - x, 2) + pow(goal_y - y, 2));

    // TASK 2.1: Basic PID to rotate towards goal
    PIDout_theta = pidAng.update(angle_to_goal, actual_angle);
    motors.setSpeeds(PIDout_theta, -PIDout_theta);
    Serial.print("X: "); Serial.println(x);
    Serial.print(" Y: "); Serial.println(y);
    Serial.print(" Theta: "); Serial.println(theta);
    Serial.print(" Distance: "); Serial.println(dist_to_goal);
    Serial.print(" PID Theta: "); Serial.println(PIDout_theta);
  /*TASK 2.2
  Improve the baseline solution by telling the robot to stop when it gets close 
  enough to the goal.
  Write your code below and comment out when moving to the next task.*/
  if (dist_to_goal < 0.01) { // cm threshold
        motors.setSpeeds(0, 0);
        return;
    }
  /*TASK 2.3
  Improve the solution further by using a second PID controller to control the velocity
  as it goes towards the goal.
  Write your code below.*/
    PIDout_distance = pidVel.update(dist_to_goal, 0.0);
    motors.setSpeeds(PIDout_distance + PIDout_theta, PIDout_distance - PIDout_theta);
    Serial.print("X: "); Serial.println(x);
    Serial.print(" Y: "); Serial.println(y);
    Serial.print(" Theta: "); Serial.println(theta);
    Serial.print(" Distance: "); Serial.println(dist_to_goal);
    Serial.print(" PID Theta: "); Serial.println(PIDout_theta);
    oled.print_odom(x, y, theta);

}
