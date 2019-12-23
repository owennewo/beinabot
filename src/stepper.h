#ifndef stepper_h
#define stepper_h
#include "Arduino.h"

// #define PIN_ENABLE 2 
// #define PIN_M0 3
// #define PIN_M1 4
// #define PIN_M2 5
// #define PIN_RESET 6
// #define PIN_SLEEP 7
// #define PIN_STEP 8
// #define PIN_DIR 9
// #define PIN_LED 13

class Stepper
{
  public:
    Stepper(int stepPin, int dirPin);
    void setMicrosteps(uint16_t microstep, int m0Pin, int m1Pin, int m2Pin);
    void begin();
    void setFrequency(int frequency);
  private:
    float _frequency;
    int _dir;
    int _dirPin;
    int _stepPin;
};
#endif