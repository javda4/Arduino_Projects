#include <Pololu3piPlus32U4.h>  // Pololu 3pi+ library for motor and encoder control
#include <Pololu3piPlus32U4Buttons.h>  // Button control library for 3pi+
using namespace Pololu3piPlus32U4;
#include "particle_filter.h"  // Particle filter library for localization
#include "odometry.h"  // Odometry library to calculate position
//#include "Map.h"  // Commented out map library, not in use
#include "PIDcontroller.h"  // PID controller library for movement control
#include <Servo.h>  // Servo library for controlling servos (sonar sensor)
#include "sonar.h"  // Sonar sensor library for distance measurements
#include <PololuBuzzer.h>  // Buzzer library for sound feedback

PololuBuzzer buzzer;  // Create a buzzer object

// Music string for playing sound (cut for brevity)
const char fugue[] PROGMEM = "...";

// Constants for map size and robot parameters
#define PI 3.14159
#define lenOfMap 36  // Map length (4 rows * 9 columns)
#define N_particles 25  // Number of particles for particle filter
#define move_noise 0.01  // Noise in movement
#define rotate_noise 0.5  // Noise in rotation
#define ultra_noise 0.1  // Noise in sonar measurements

// Robot geometry and encoder specifications
#define diaL 3.2  // Diameter of the left wheel (in cm)
#define diaR 3.2  // Diameter of the right wheel (in cm)
#define nL 12  // Number of encoder ticks for the left wheel per rotation
#define nR 12  // Number of encoder ticks for the right wheel per rotation
#define w 9.6  // Width of the robot (in cm)
#define gearRatio 75  // Gear ratio of the robot's motors
#define deadreckoning false  // Whether to use dead reckoning for navigation (false = no)

#define minOutputAng -100  // Minimum output angle for PID
#define maxOutputAng 100  // Maximum output angle for PID
#define kpAng 200  // Proportional gain for PID
#define kdAng 0.05  // Derivative gain for PID
#define kiAng 5  // Integral gain for PID
#define clamp_iAng 50  // Integral clamp for PID
#define base_speedAng 50  // Base speed for angular movement

// Create objects for motors, servos, encoders, and odometry
Motors motors;
Servo servo;
Encoders encoders;
Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, deadreckoning);  // Initialize odometry object
ParticleFilter particle(lenOfMap, N_particles, move_noise, rotate_noise, ultra_noise, 0);  // Initialize particle filter
PIDcontroller pidAng(kpAng, kiAng, kdAng, minOutputAng, maxOutputAng, clamp_iAng);  // Initialize PID controller

// Odometry and encoder variables
int16_t deltaL = 0, deltaR = 0;
int16_t encCountsLeft = 0, encCountsRight = 0;
float x, y, theta;  // Robot's position (x, y) and orientation (theta)
double PIDout_theta;  // Output of the PID controller for angular movement
float x_last = 0.0, y_last = 0.0, theta_last = 0.0;  // Last position and orientation for delta calculation

// Map dimensions
const int ROWS = 4;
const int COLS = 9;
static bool visited[ROWS][COLS] = {false};  // Array to track visited grid cells

const int distance = 1500;  // Distance to move forward (in milliseconds)

// Struct to hold distances from sonar sensor
struct Distances {
  int front;
  int left;
  int right;
};

// Function to check distances using sonar sensor
Distances checkSurroundings() {
  Distances d;
  servo.write(90); delay(1000); d.front = sonar.readDist(); delay(1000);
  Serial.print("Front: ");
  Serial.println(d.front);
  servo.write(180); delay(1000); d.left = sonar.readDist(); delay(1000);
  Serial.print("Left: ");
  Serial.println(d.left);
  servo.write(0); delay(1000); d.right = sonar.readDist(); delay(1000);
  Serial.print("Right: ");
  Serial.println(d.right);
  servo.write(90); delay(1000);
  return d;
}

// Function to rotate the robot to a specific angle
void rotateTo(float target_angle) {
  while (true) {
    // Get encoder counts and update odometry
    deltaL = encoders.getCountsAndResetLeft();
    deltaR = encoders.getCountsAndResetRight();
    encCountsLeft += deltaL;
    encCountsRight += deltaR;
    odometry.update_odom(encCountsLeft, encCountsRight, x, y, theta);

    // Update the PID controller to compute the new speed
    PIDout_theta = pidAng.update(theta, target_angle);
    motors.setSpeeds(PIDout_theta, -PIDout_theta);  // Apply PID output to motors

    // Break when the robot is close enough to the target angle
    if (abs(target_angle - theta) < 0.1) {
      motors.setSpeeds(0, 0);  // Stop the motors
      break;
    }
  }
}

// Function to move the robot forward for a given duration at a given speed
void moveForward(int duration, int speed) {
  motors.setSpeeds(speed, speed);  // Set motor speeds
  delay(duration);  // Wait for the duration
  motors.setSpeeds(0, 0);  // Stop the motors
}

// Function to print out the accessible squares (visited cells) on the map
void printAccessibleSquares() {
  Serial.println("Accessible squares:");
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      if (visited[i][j]) {
        Serial.print("("); Serial.print(i); Serial.print(", "); Serial.print(j); Serial.println(")");
      }
    }
  }
}

// Counter variable for debugging
int i = 0;

// Depth-first search (DFS) function to explore the map
void exploreDFS() {
  static int stack[lenOfMap][2];  // Stack to hold the current exploration path
  static int top = -1;  // Top pointer for the stack

  int currX = (int)(x);  // Current x position (row)
  int currY = (int)(y);  // Current y position (column)

  // Return if the current position is out of bounds or already visited
  if (currX < 0 || currX >= ROWS || currY < 0 || currY >= COLS || visited[currX][currY]) return;

  Serial.print(i);  // Print debug info
  i += 1;
  visited[currX][currY] = true;  // Mark the current position as visited
  stack[++top][0] = currX;  // Push current position onto stack
  stack[top][1] = currY;

  Distances d = checkSurroundings();  // Check surroundings using sonar

  // Move forward if the front is clear and the next column (Y) is within bounds
  if (d.front > 24 && currY + 1 < COLS && !visited[currX][currY + 1]) {
    moveForward(distance, 100);  // Move forward
    exploreDFS();  // Explore further
    rotateTo(theta + PI); moveForward(distance, 100); rotateTo(theta + PI);  // Backtrack
  }

  // Turn right 90 degrees (clockwise) and move forward if the left is clear and the next row (X) is within bounds
  if (d.left > 24 && currX + 1 < ROWS && !visited[currX + 1][currY]) {
    rotateTo(theta + (PI / 2) - 0.4);  // Turn right 90 degrees
    moveForward(distance, 100);  // Move forward
    exploreDFS();  // Explore further
    rotateTo(theta + PI); moveForward(distance, 100); rotateTo(theta + PI);  // Backtrack
  }

  // Turn left 90 degrees (counterclockwise) and move forward if the right is clear and the previous row (X) is within bounds
  if (d.right > 24 && currX - 1 >= 0 && !visited[currX - 1][currY]) {
    Serial.println("ENTERED RIGHT!!!");  // Debugging message
    moveForward(distance, 100);  // Move forward
    moveForward(distance, -100);  // Move backward
    rotateTo(theta - (PI / 2));  // Turn left 90 degrees
    moveForward(distance, 100);  // Move forward
    exploreDFS();  // Explore further
    rotateTo(theta + PI); moveForward(distance, 100); rotateTo(theta + PI);  // Backtrack
  }

  top--;  // Pop from the stack to backtrack
}

void setup() {
  Serial.begin(9600);  // Start serial communication for debugging
  servo.attach(5);  // Attach the servo to pin 5
  servo.write(90);  // Set the servo to the center position
  delay(1000);  // Wait for a second
}

void loop() {
  // Get encoder counts and update the odometry
  deltaL = encoders.getCountsAndResetLeft();
  deltaR = encoders.getCountsAndResetRight();
  encCountsLeft += deltaL;
  encCountsRight += deltaR;
  odometry.update_odom(encCountsLeft, encCountsRight, x, y, theta);

  // Calculate deltas for position
  float dx = x - x_last, dy = y - y_last, dtheta = theta - theta_last;
  x_last = x; y_last = y; theta_last = theta;

  exploreDFS();  // Start exploring using DFS
  printAccessibleSquares();  // Print the accessible squares
  Serial.print("HOWDY!!!!!!!");  // Debugging message
  delay(1000);  // Wait for a second before the next iteration
}
