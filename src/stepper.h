#ifndef motor_h
#define motor_h
#include "Arduino.h"

#define PIN_ENABLE 2 
#define PIN_M0 3
#define PIN_M1 4
#define PIN_M2 5
#define PIN_RESET 6
#define PIN_SLEEP 7
#define PIN_STEP 8
#define PIN_DIR 9
// #define PIN_LED 13

class Stepper
{
  public:
    Stepper(int stepPin, int dirPin);
    void setMicrosteps(uint16_t microsteps);
    void begin();
    void setFrequency(int frequency);
  private:
    float _frequency;
};
#endif