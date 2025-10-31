#ifndef PrintOLED_h
#define PrintOLED_h
#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

class PrintOLED{
  public:
    PrintOLED();
    void print_encoder(float L, float R);
    void print_float(float x);
    
  private:
    int _lastUpdateTime;
};

#endif
