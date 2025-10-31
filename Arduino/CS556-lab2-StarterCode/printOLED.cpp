#include <Pololu3piPlus32U4.h>
#include "printOLED.h"
using namespace Pololu3piPlus32U4;

OLED display;

PrintOLED::PrintOLED() {
  _lastUpdateTime = millis() - 100; // timer for displaying to screen
}

void PrintOLED::print_encoder(float L, float R){
      char bufL[6];
      char bufR[6];
            
      // Update the OLED screen every 50 ms.
      if ((uint16_t)(millis() - _lastUpdateTime) > 50)
      {
        _lastUpdateTime = millis();
        dtostrf(L,4,0,bufL);
        dtostrf(R,4,0,bufR);
      }
      
      display.gotoXY(0, 0);
      display.print(bufL);
      display.gotoXY(2, 1);
      display.print(bufR);
}

void PrintOLED::print_float(float x){
      char buf[8];
            
      // Update the OLED screen every 250 ms.
      if ((uint16_t)(millis() - _lastUpdateTime) > 250)
      {
        _lastUpdateTime = millis();
        dtostrf(x,4,2,buf);
      }

      display.gotoXY(0, 0);
      display.print(buf);
}
