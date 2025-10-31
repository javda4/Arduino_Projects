#include <Pololu3piPlus32U4.h>
#include <Pololu3piPlus32U4Buttons.h>
using namespace Pololu3piPlus32U4;
#include "particle_filter.h" 
#include "odometry.h"
#include "Map.h"
#include "PIDcontroller.h"
#include <Servo.h>
#include "sonar.h"
#include <PololuBuzzer.h>

PololuBuzzer buzzer;

const char fugue[] PROGMEM = "..."; // music string cut for brevity

#define PI 3.14159
#define lenOfMap 144
#define N_particles 25
#define move_noise  0.01
#define rotate_noise 0.5
#define ultra_noise 0.1

#define diaL 3.2
#define diaR 3.2
#define nL 12
#define nR 12
#define w 9.6
#define gearRatio 75
#define deadreckoning false

#define minOutputAng -100
#define maxOutputAng 100
#define kpAng 200
#define kdAng 0.05
#define kiAng 5
#define clamp_iAng 50
#define base_speedAng 50

Motors motors;
Servo servo;
Encoders encoders;
Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, deadreckoning);
ParticleFilter particle(lenOfMap, N_particles, move_noise, rotate_noise, ultra_noise, 0);
PIDcontroller pidAng(kpAng, kiAng, kdAng, minOutputAng, maxOutputAng, clamp_iAng);

Map map = Map();

int16_t deltaL=0, deltaR=0;
int16_t encCountsLeft = 0, encCountsRight = 0;
float x, y, theta;
double PIDout_theta;
float x_last = 0.0, y_last = 0.0, theta_last = 0.0;

struct Distances {
  int front;
  int left;
  int right;
};

Distances checkSurroundings() {
  Distances d;
  servo.write(90); delay(300); d.front = sonar.readDist(); delay(10);
  servo.write(180); delay(300); d.left = sonar.readDist(); delay(10);
  servo.write(0); delay(300); d.right = sonar.readDist(); delay(10);
  servo.write(90); delay(300);
  return d;
}

void rotateTo(float target_angle) {
  while (true) {
    deltaL = encoders.getCountsAndResetLeft();
    deltaR = encoders.getCountsAndResetRight();
    encCountsLeft += deltaL;
    encCountsRight += deltaR;
    odometry.update_odom(encCountsLeft, encCountsRight, x, y, theta);
    PIDout_theta = pidAng.update(theta, target_angle);
    motors.setSpeeds(-PIDout_theta, PIDout_theta);
    if (abs(target_angle - theta) < 0.1) {
      motors.setSpeeds(0, 0);
      break;
    }
  }
}

void moveForward(int duration, int speed) {
  motors.setSpeeds(speed, speed);
  delay(duration);
  motors.setSpeeds(0, 0);
}

void exploreDFS() {
  static bool visited[12][12] = {false};
  static int stack[144][2];
  static int top = -1;

  int currX = (int)(x);
  int currY = (int)(y);

  if (visited[currX][currY]) return;
  visited[currX][currY] = true;

  stack[++top][0] = currX;
  stack[top][1] = currY;

  Distances d = checkSurroundings();

  if (d.front > 24 && !visited[currX + 1][currY]) {
    moveForward(1000, 100);
    exploreDFS();
    rotateTo(theta + PI); moveForward(1000, 100); rotateTo(theta + PI); // backtrack
  }
  if (d.left > 24 && !visited[currX][currY + 1]) {
    rotateTo(theta + PI/2); moveForward(1000, 100); exploreDFS();
    rotateTo(theta + PI); moveForward(1000, 100); rotateTo(theta + PI); // backtrack
  }
  if (d.right > 24 && !visited[currX][currY - 1]) {
    rotateTo(theta - PI/2); moveForward(1000, 100); exploreDFS();
    rotateTo(theta + PI); moveForward(1000, 100); rotateTo(theta + PI); // backtrack
  }

  top--;
}

void setup() {
  Serial.begin(9600);
  servo.attach(5);
  servo.write(90);
  delay(1000);
}

void loop() {
  deltaL = encoders.getCountsAndResetLeft();
  deltaR = encoders.getCountsAndResetRight();
  encCountsLeft += deltaL;
  encCountsRight += deltaR;
  odometry.update_odom(encCountsLeft, encCountsRight, x, y, theta);
  float dx = x - x_last, dy = y - y_last, dtheta = theta - theta_last;
  particle.move_particles(dx, dy, dtheta);
  particle.measure();
  x_last = x; y_last = y; theta_last = theta;

  exploreDFS();
  delay(1000);
}
