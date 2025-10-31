#include <Pololu3piPlus32U4.h>
#include "printOLED.h"
#include "PIDcontroller.h"
#include "PDcontroller.h"
#include "odometry.h" // Uncomment if using odometry

using namespace Pololu3piPlus32U4;

Motors motors;
Encoders encoders;


// Odometry Parameters
#define diaL 3.2
#define diaR  3.2
#define nL 12
#define nR 12
#define w 9.6
#define gearRatio 75
#define DEAD_RECKONING false 

// Tune these based on testing
#define minOutput -100
#define maxOutput 100
#define kp 1.0  // Adjust Kp
#define kd 0.5  // Adjust Kd
#define ki 0.1  // Adjust Ki
#define clamp_i 10  // Adjust integral clamp
#define base_speed 50

// PD and PID Controllers
PDcontroller pdcontroller(kp, kd, minOutput, maxOutput);
PIDcontroller pidcontroller(kp, ki, kd, minOutput, maxOutput, clamp_i);
Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, DEAD_RECKONING);
// Given goals in radians


const float goal_theta = 3.14;  // Example: PI radians

// Odometry variables
int16_t deltaL = 0, deltaR = 0;
int16_t encCountsLeft = 0, encCountsRight = 0;
float x, y, theta;

// Controller output variables
double PDout, PIDout;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Read encoder data
  deltaL = encoders.getCountsAndResetLeft();
  deltaR = encoders.getCountsAndResetRight();
  encCountsLeft += deltaL;
  encCountsRight += deltaR;

  // update odometry
  odometry.update_odom(encCountsLeft, encCountsRight, x, y, theta);


  /* TASK 2.2 
  PDout = pdcontroller.update(theta, goal_theta);
  motors.setSpeeds(PDout, PDout);  // Adjust motor direction if needed
  Serial.print("Theta: ");
  Serial.print(theta);
  Serial.print(" PD Output: ");
  Serial.println(PDout);
  */



  /* TASK 3.2 - Utilize PID Controller */
  PIDout = pidcontroller.update(theta, goal_theta);
  motors.setSpeeds(PIDout, -PIDout);  // Adjust motor direction if needed

  // Print data for debugging
  Serial.print("Theta: ");
  Serial.print(theta);
  Serial.print(" PID Output: ");
  Serial.println(PIDout);



  delay(100);  // Small delay for stability
}
