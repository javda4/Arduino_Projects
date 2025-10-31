#ifndef Pcontroller_h
#define Pcontroller_h
#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

class Pcontroller{
  public:
    Pcontroller(float kp, double minOutput, double maxOutput);
    double update(double value, double target_value); 
    
  private:
    float _kp;               // Proportional gain
    double _minOutput;       // Minimum output limit
    double _maxOutput;       // Maximum output limit
    double _error;           // Difference between target and current value
    double _output;          // Raw controller output
    double _clampOut;        // Clamped controller output
	
};

#endif
