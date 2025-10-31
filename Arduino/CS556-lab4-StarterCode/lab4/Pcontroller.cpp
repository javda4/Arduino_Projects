#include <Pololu3piPlus32U4.h>
#include "Pcontroller.h"
using namespace Pololu3piPlus32U4;

Pcontroller::Pcontroller(float kp, double minOutput, double maxOutput) {
    _kp = kp;
    _minOutput = minOutput;
    _maxOutput = maxOutput;
    _error = 0.0;
    _output = 0.0;
    _clampOut = 0.0;
}

double Pcontroller::update(double value, double target_value){
    _error = target_value - value;   // Calculate error
    _output = _kp * _error;          // Apply proportional gain
    
    // Clamp the output within the specified limits
    if (_output > _maxOutput) {
        _clampOut = _maxOutput;
    } else if (_output < _minOutput) {
        _clampOut = _minOutput;
    } else {
        _clampOut = _output;
    }

    return _clampOut;
}







