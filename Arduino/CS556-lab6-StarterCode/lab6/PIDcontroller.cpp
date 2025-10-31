#include <Arduino.h>  // For millis()
#include <Pololu3piPlus32U4.h>
#include "PIDcontroller.h"
using namespace Pololu3piPlus32U4;

PIDcontroller::PIDcontroller(float kp, float ki, float kd, double minOutput, double maxOutput, double clamp_i) {
    _kp = kp;
    _ki = ki;
    _kd = kd;
    _minOutput = minOutput;
    _maxOutput = maxOutput;
    _clampI = clamp_i;
    _error = 0;
    _prevError = 0;
    _output = 0;
    _clampOut = 0;
    _integral = 0;
    _prevTime = millis();  // Initialize time tracking
}

double PIDcontroller::update(double value, double target_value) {
    unsigned long currentTime = millis();  // Get current time in milliseconds
    double deltaTime = (currentTime - _prevTime) / 1000.0;  // Convert to seconds

    // Avoid division by zero (on the first call)
    if (deltaTime == 0) {
        deltaTime = 1e-6;  // A very small number to prevent division by zero
    }

    // Calculate the error
    _error = target_value - value;

    // Integral term (accumulate error over time)
    _integral += _error * deltaTime;
    
    // Clamp integral term to prevent windup
    _integral = constrain(_integral, -_clampI, _clampI);

    // Derivative term (rate of change of error)
    double derivative = (_error - _prevError) / deltaTime;

    // Compute PID output
    _output = (_kp * _error) + (_ki * _integral) + (_kd * derivative);

    // Clamp output to the min and max range
    _clampOut = constrain(_output, _minOutput, _maxOutput);

    // Store values for next iteration
    _prevError = _error;
    _prevTime = currentTime;

    return _clampOut;
}
