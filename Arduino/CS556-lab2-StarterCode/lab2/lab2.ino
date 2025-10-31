#include <Pololu3piPlus32U4.h>
#include <Servo.h>
#include "sonar.h"
#include "printOLED.h"
using namespace Pololu3piPlus32U4;
Motors motors;
Servo servo; 
Sonar sonar(4);
PrintOLED printOLED;

float dist=0;

void setup() {
  Serial.begin(9600);
  servo.attach(5); // Servo attached to pin #5
  servo.write(90);
  delay(15);

}

void loop() {  
      // Read data from sonar sensor
      dist = sonar.readDist(); // distance given in cm
      printOLED.print_float(dist); // print to OLED screen     
      Serial.println(dist); //print to Tools>Serial Monitor
      // Turn servo to set location
      // servo.write(0);
      // delay(2000);  
      // servo.write(180);
      // delay(2000); 
      // servo.write(90);
      // delay(2000);
}

      
