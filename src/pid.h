#ifndef pid_h
#define pid_h
#include "Arduino.h"

class PID
{
  public:
    PID(float kp, float ki, float kd);
    float calculate(float error, float offset);
  private:
    float _kp, _ki, _kd;
    float _errorSum;
    long _lastTime;
    long _lastError;
};
#endif