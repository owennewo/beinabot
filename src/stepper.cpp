#include "Arduino.h"
#include <stepper.h>

Stepper::Stepper(int stepPin, int dirPin)
{
      _stepPin = stepPin;
      _dirPin = dirPin;
}

void Stepper::setMicrosteps(uint16_t microstep, int m0Pin, int m1Pin, int m2Pin) 
{
    if (microstep == 1) {
        digitalWrite(m0Pin, LOW);
        digitalWrite(m1Pin, LOW);
        digitalWrite(m2Pin, LOW);      
    } else if (microstep == 2) {
        digitalWrite(m0Pin, HIGH);
        digitalWrite(m1Pin, LOW);
        digitalWrite(m2Pin, LOW);          
    } else if (microstep == 4) {
        digitalWrite(m0Pin, LOW);
        digitalWrite(m1Pin, HIGH);
        digitalWrite(m2Pin, LOW);          
    } else if (microstep == 8) {
        digitalWrite(m0Pin, HIGH);
        digitalWrite(m1Pin, HIGH);
        digitalWrite(m2Pin, LOW);          
    } else if (microstep == 16) {
        digitalWrite(m0Pin, LOW);
        digitalWrite(m1Pin, LOW);
        digitalWrite(m2Pin, HIGH);          
    } else if (microstep == 32) {
        digitalWrite(m0Pin, HIGH);
        digitalWrite(m1Pin, HIGH);
        digitalWrite(m2Pin, HIGH);          
    }
}

void Stepper::begin() 
{
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                     GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                      GCLK_GENCTRL_GENEN |         // Enable GCLK4
                      GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
                      GCLK_GENCTRL_ID(4);          // Select GCLK4
  while (GCLK->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  // Enable the port multiplexer for the 2 PWM channels: timer TCC0 outputs CC2 and CC3 on D10 and D12
  PORT->Group[g_APinDescription[10].ulPort].PINCFG[g_APinDescription[10].ulPin].bit.PMUXEN = 1;
  PORT->Group[g_APinDescription[12].ulPort].PINCFG[g_APinDescription[12].ulPin].bit.PMUXEN = 1;
 
  // Connect the TCC timers to the port outputs - port pins are paired odd PMUO and even PMUXE
  // F & E specify the timers: TCC0, TCC1 and TCC2
  PORT->Group[g_APinDescription[10].ulPort].PMUX[g_APinDescription[10].ulPin >> 1].reg = PORT_PMUX_PMUXO_F | PORT_PMUX_PMUXE_F;
 
  // Feed GCLK4 to TCC0 and TCC1
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TCC0 and TCC1
                      GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                      GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed GCLK4 to TCC0 and TCC1
  while(GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization 

  TCC0->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;          // Single slope PWM operation         
  while (TCC0->SYNCBUSY.bit.WAVE);                 // Wait for synchronization
 
  TCC0->PER.reg = 4799;                            // Set the frequency of the PWM on TCC0 to 10kHz
  while (TCC0->SYNCBUSY.bit.PER);                  // Wait for synchronization

  TCC0->CC[2].reg = 2399;                          // TCC0 CC2 - 50% duty-cycle on D10
  while (TCC0->SYNCBUSY.bit.CC2);                  // Wait for synchronization
  TCC0->CC[3].reg = 2399;                          // TCC0 CC3 - 50% duty-cycle on D12
  while (TCC0->SYNCBUSY.bit.CC3);                  // Wait for synchronization
 
  // Divide the 48MHz signal by 1 giving 48MHz (20.83ns) TCC0 timer tick and enable the timer
  TCC0->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV64 |    // Divide GCLK4 by 1
                     TCC_CTRLA_ENABLE;             // Enable the TCC0 output
  while (TCC0->SYNCBUSY.bit.ENABLE);               // Wait for synchronization

}

int Stepper::setFrequency(float frequency) 
{
    if (frequency == 0) frequency = 1;
    if (frequency < 0 ) {
        _dir = HIGH;
    } else {
        _dir = LOW;
    }

    int period = 4799 / abs(frequency);
    if (period > 9599) period = 9599;
    int halfPeriod = period / 2;

    TCC0->PERB.reg = period;                           // Set the frequency to 10kHz
    while (TCC0->SYNCBUSY.bit.PERB);                 // Wait for synchronization
    TCC0->CCB[2].reg = halfPeriod;                         // Set the duty cycle to 50% on channel 2
    while (TCC0->SYNCBUSY.bit.CCB2);                 // Wait for synchronization
    TCC0->CCB[3].reg = halfPeriod;                         // Set the duty cycle to 50% on channel 3
    while (TCC0->SYNCBUSY.bit.CCB3);                 // Wait for synchronization

    digitalWrite(_dirPin, _dir);
    return period;
    // MyTimer5.changeFrequency(abs(frequency) * 2);

}