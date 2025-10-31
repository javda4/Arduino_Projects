#include <Pololu3piPlus32U4.h>
#include <Pololu3piPlus32U4Buttons.h>
using namespace Pololu3piPlus32U4;
#include "particle_filter.h"
#include "odometry.h"
#include "Map.h"

ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;

#define PI 3.14159

#define lenOfMap 144
#define N_particles 25
#define move_noise  0.1
#define rotate_noise 0.5
#define ultra_noise 0.1

#define diaL 3.2 //define physical robot parameters
#define diaR  3.2
#define nL 12
#define nR 12
#define w 9.6
#define gearRatio 75
#define dead_reckoning false

Motors motors;
Encoders encoders;

Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, dead_reckoning);
ParticleFilter particle(lenOfMap, N_particles, move_noise, rotate_noise, ultra_noise);

uint8_t iter =0;

//odometry
int16_t deltaL=0, deltaR=0;
int16_t encCountsLeft = 0, encCountsRight = 0;
float x, y, theta;
float x_last = 0.0;
float y_last = 0.0;
float theta_last = 0.0;

void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

  Map map = Map();

  //This is an example of how to estimate the distance to a wall for the given
  //map, assuming the robot is at (0, 0) and has heading PI
  float origin[2] = {0.5,0.5};
  float theta = PI;
  float closestDist = map.closest_distance(origin,theta);  

  Serial.println(closestDist);
  
}

void loop() {
  movement();

  //Get odometer readings  
  deltaL = encoders.getCountsAndResetLeft();
  deltaR = encoders.getCountsAndResetRight();
  encCountsLeft += deltaL;
  encCountsRight += deltaR;   
  odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta);

  float dx = x - x_last;
  float dy = y - y_last;
  float dtheta = theta - theta_last;

  //movement function
  //Propagate particles by using move_particles.
  //Parameters are change from current and past odometer values 
  //TODO: Put code under here 
  particle.move_particles(dx, dy, dtheta);


  //Measaure, estimation, and resample
  //Calculate particle's posterior probabilities, calculate estimated robot's position, and resample
  //TODO: Put code under here 
  particle.measure();


  // Display all particle locations and estimated robot location on screen   
  //TODO: Put code under here 
  particle.print_particles();
  
    
  //save last odometer reading
  //TODO: Fill in "..."
  x_last = x;
  y_last = y;
  theta_last = theta;
  
  iter++;
    
  delay(3000); //for easier viewing of output
 
}

// void movement(){
//    if (buttonA.isPressed()) {
//     motors.setSpeeds(-200, 200); // left turn
//     delay(415);
//     motors.setSpeeds(0, 0);
//     Serial.print("Left pressed!\n");
//   } 
  
//   else if (buttonB.isPressed()) {
//     motors.setSpeeds(200, 200); // forward
//     delay(1000);
//     motors.setSpeeds(0, 0);
//     Serial.print("Forward pressed!\n");
//   } 
  
//   else if (buttonC.isPressed()) {
//     motors.setSpeeds(200, -200); // right turn
//     delay(415);
//     motors.setSpeeds(0, 0);
//     Serial.print("Right pressed!\n");
//   }
 
// }

//movement using keyboard inputs to save my back
void movement() {
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'a') {
      motors.setSpeeds(-200, 200); // left turn
      delay(415);
      motors.setSpeeds(0, 0);
      Serial.println("Left pressed!");
    } 
    else if (command == 'w') {
      motors.setSpeeds(200, 200); // forward
      delay(1000);
      motors.setSpeeds(0, 0);
      Serial.println("Forward pressed!");
    } 
    else if (command == 'd') {
      motors.setSpeeds(200, -200); // right turn
      delay(415);
      motors.setSpeeds(0, 0);
      Serial.println("Right pressed!");
    }
  }
}
