#include <Pololu3piPlus32U4.h>
#include <Servo.h>
#include "printOLED.h"
#include "odometry.h"
#include "my_robot.h"
using namespace Pololu3piPlus32U4;
//Motors motors;
Encoders encoders;
MyRobot robot;
PrintOLED oled;

/*DEFINE VARIBLES FOR PHYSICAL ROBOT PARAMETRS 
BY INSERTING THE PARAMETER VALUES FROM THE ROBOT DOCUMENTATION*/
#define DEAD_RECKONING false // if "false": uses formula from the lecture slides to calculate theta. if "true", uses IMU data to calculate theta.
#define diaL 3.2 //define physical robot parameters (cm)
#define diaR  3.2
#define nL 12
#define nR 12
#define w 9.6 //cm
#define gearRatio 75

#define BaseSpeed 100  // unit: mm per second


Odometry odometry(diaL, diaR, w, nL, nR, gearRatio, DEAD_RECKONING);


int16_t deltaL=0, deltaR=0;
int16_t encCountsLeft = 0, encCountsRight = 0;

float x = 0.0, y = 0.0, theta = 0.0;

void setup() {
  Serial.begin(9600);
  delay(10);
}

void loop() {  
   while (Serial.available() == 0) {
    Serial.println("Waiting..."); //wait for us to be ready to start program with any user input
    delay(1000);
  }
      //(TASK 2.1) Test Encoders while Staying Still 

      //IMPORT FUNCTIONS FROM LAB1 (MOVE FORWARD, TURN LEFT, ...) 
      //TO ENABLE ROBOT TO MOVE ONE METER FORWARD, CLOCKWISE AND COUNTERCLOCKWISE

      //Encoder code should always be before updating and movement
      // Read data from encoders
                deltaL = encoders.getCountsAndResetLeft();
                deltaR = encoders.getCountsAndResetRight();
                encCountsLeft += deltaL;
                encCountsRight += deltaR;

                Serial.println("before left:  "); 
                Serial.println(encCountsLeft); 
                Serial.println("before right:  ");
                Serial.println(encCountsRight);
                

                // robot.forward(1, BaseSpeed);  // Move forward 1 meter //////////////////////////
                deltaL = encoders.getCountsAndResetLeft();
                deltaR = encoders.getCountsAndResetRight();

      // Increment total encoder count
                encCountsLeft += deltaL;
                encCountsRight += deltaR;

                Serial.print(" left:  "); 
                Serial.println(encCountsLeft); 
                Serial.println(" right:  ");
                Serial.println(encCountsRight);
                oled.print_encoder(encCountsLeft, encCountsRight);
      
                //while (true);  // Stops further execution

      //(TASK 2.2) TEST ENCODERS WHILE MOVING EACH OF THE THREE MOVEMENTS LISTED BELOW (ONE AT A TIME) 
      //(NOTE: YOU SHOULD UPDATE AND USE PRIMITIVE FUNCTIONS FROM LAB1)
      //DO NOT DELETE CODE AFTER EACH TASK, COMMENT OUT INSTEAD

      
      //(2.2a) MOVE FORWARD ON A STRAIGHT LINE FOR ONE METER


      //  robot.forward(1, BaseSpeed); // Move backward 1 meter //////////////////////////
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;
      //  oled.print_encoder(encCountsLeft,encCountsRight);
      //  while(true);



      //(2.2b) MOVE BACKWARD ON A STRAIGHT LINE FOR ONE METER
      //  robot.backward(1, BaseSpeed); // Move backward 1 meter //////////////////////////
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;
      //  oled.print_encoder(encCountsLeft,encCountsRight);
      //     while(true);



      
      //(2.2c) TURN LEFT FOR 90 DEGREES

      //  robot.forward(1,BaseSpeed);
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;
      //  robot.spin_right(720, BaseSpeed); // Turn left 90 degrees
      //  oled.print_encoder(encCountsLeft,encCountsRight);

      // while(true);


      //(TASK 3.1) IMPLEMENT ODOMETRY 

      /*UNCOMMENT Odometry.update_odom DOWN BELOW*/
      //odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta); //calculate robot's position

      /*NOW GO TO odometry.cpp file AND COMPLETE THE udate_odom FUNCTION. 
      IN odometry.cpp, ADD CODES TO ENABLE THE ROBOT TO 
      TO CALCULATE x, y, theta, AND
      TO PRINT THE CALCULATED VALUES TO OLED SCREEN AND SERIAL MONITOR*/
      

      //TEST ODOMETRY WHILE MOCING EACH OF THE THREE MOVEMENTS LISTED BELOW (ONE AT A TIME)
      //DO NOT DELETE CODE AFTER EACH TASK, COMMENT OUT INSTEAD

      //(3.2a)  15-meter straight line down the hallway

      //  robot.forward(15, BaseSpeed); // Move backward 1 meter //////////////////////////
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
     //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;
      //  odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta);
      //  oled.print_odom(x,y,theta);
      //  while(true);  // stop program after execution




      //(3.2c)  1-meter square clockwise

      // int gos = 0;
      //     while(gos < 4){    //create square with loop
      //  robot.forward(1,BaseSpeed);
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;

      //  odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta);
      //  oled.print_odom(x,y,theta);
      //  robot.spin_right(700, BaseSpeed); // Turn left 90 degrees
      //  gos = gos + 1;
      //     }





      //(3.3e)  1-meter square counterclockwise

      // int gos = 0;
      //     while(gos < 4){  //create square with loop
      //  robot.forward(1,BaseSpeed);
      //  deltaL = encoders.getCountsAndResetLeft();
      //  deltaR = encoders.getCountsAndResetRight();
      //  encCountsLeft += deltaL;
      //  encCountsRight += deltaR;

      //  odometry.update_odom(encCountsLeft,encCountsRight, x, y, theta);
      //  oled.print_odom(x,y,theta);
      //  robot.spin_left(700, BaseSpeed); // Turn left 90 degrees
      //  gos = gos + 1;
      //     }
}
