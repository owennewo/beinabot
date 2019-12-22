#include "Arduino.h"
#include <motor.h>
#include <timer5.h>

void timer(void) {
    static bool on = false;

    
    
    // count++;  // count number of toggles
    if (on == true) {
      on = false;
    //   Serial.println("*********** LOW");
        digitalWrite(PIN_LED,LOW);
        digitalWrite(PIN_STEP,LOW);
    } else {
      on = true;
      Serial.println("*********** STEP");
        digitalWrite(PIN_LED,HIGH);
        digitalWrite(PIN_STEP,HIGH);
    }
}

Motor::Motor(int microsteps)
{
      
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_M0, OUTPUT);
  pinMode(PIN_M1, OUTPUT);
  pinMode(PIN_M2, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_SLEEP, OUTPUT);
  pinMode(PIN_DIR, OUTPUT);
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  digitalWrite(PIN_ENABLE, LOW);
  digitalWrite(PIN_M0, LOW);
  digitalWrite(PIN_M1, LOW);
  digitalWrite(PIN_M2, LOW);
  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_SLEEP, HIGH);
  digitalWrite(PIN_DIR, LOW);
  digitalWrite(PIN_STEP, LOW);

}

void Motor::setMicrosteps(uint16_t microstep) 
{
    if (microstep == 1) {
        digitalWrite(PIN_M0, LOW);
        digitalWrite(PIN_M1, LOW);
        digitalWrite(PIN_M2, LOW);      
    } else if (microstep == 2) {
        digitalWrite(PIN_M0, HIGH);
        digitalWrite(PIN_M1, LOW);
        digitalWrite(PIN_M2, LOW);          
    } else if (microstep == 4) {
        digitalWrite(PIN_M0, LOW);
        digitalWrite(PIN_M1, HIGH);
        digitalWrite(PIN_M2, LOW);          
    } else if (microstep == 8) {
        digitalWrite(PIN_M0, HIGH);
        digitalWrite(PIN_M1, HIGH);
        digitalWrite(PIN_M2, LOW);          
    } else if (microstep == 16) {
        digitalWrite(PIN_M0, LOW);
        digitalWrite(PIN_M1, LOW);
        digitalWrite(PIN_M2, HIGH);          
    } else if (microstep == 32) {
        digitalWrite(PIN_M0, HIGH);
        digitalWrite(PIN_M1, HIGH);
        digitalWrite(PIN_M2, HIGH);          
    }
}

void Motor::begin() 
{

    MyTimer5.begin(1);

  // define the interrupt callback function
  MyTimer5.attachInterrupt(timer);
  
  // start the timer
  MyTimer5.start();
    Serial.println("*********** begin complete");
}

void Motor::setFrequency(int frequency) 
{
    if (frequency ==0) frequency = 1;
    if (frequency < 0 ) {
        digitalWrite(PIN_DIR, LOW);
    } else {
        digitalWrite(PIN_DIR, HIGH);
    }



    MyTimer5.changeFrequency(abs(frequency) * 2);

  // define the interrupt callback function
//   MyTimer5.attachInterrupt(timer);
  
//   // start the timer
//   MyTimer5.start();

}