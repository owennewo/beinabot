#include "Arduino.h"
#include <pid.h>

PID::PID(float kp, float ki, float kd)
{
  _kp = kp;
  _ki = ki;
  _kd  = kd;
  _lastTime = millis() -1; //subtract 1 to avoid divide by zero
}

float PID::calculate(float error, float offset) {

    float offsetError = (error - offset);

    float p = offsetError * _kp;
    _errorSum +=  offsetError;
    float i =  _errorSum * _ki;
    long now = millis();
    long // timeDiff =  now - _lastTime;
    _lastTime = now;
    float d = (offsetError - _lastError) / _lastTime * _kd;
    return p + i + d;
}
  