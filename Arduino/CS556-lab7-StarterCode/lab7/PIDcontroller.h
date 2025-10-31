#ifndef PIDcontroller_h
#define PIDcontroller_h
#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

class PIDcontroller {
  public:
    PIDcontroller(float kp, float ki, float kd, double minOutput, double maxOutput, double clamp_i);
    double update(double value, double target_value);
    
  private:
    float _kp;               // Proportional gain
    float _ki;               // Integral gain
    float _kd;               // Derivative gain
    double _minOutput;       // Minimum output limit
    double _maxOutput;       // Maximum output limit
    double _error;           // Difference between target and current value
    double _prevError;       // Previous error for derivative calculation
    double _output;          // Raw controller output
    double _clampOut;        // Clamped controller output
    unsigned long _prevTime; // Time tracking for PID updates
    double _clampI;          // Clamp for integral component
    double _integral;        // Accumulated error for integral term
};

#endif
