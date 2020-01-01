#include "sharedtimer.h"

SharedTimer* SharedTimer::instances[MAX_INSTANCES] = { nullptr };
int SharedTimer::size_timers = 0;
float rpm = -1.0;

SharedTimer::SharedTimer (functionPtr action, float newRpm) {
    // Serial.println("create timer");
    callback = action;
    instances[size_timers] = this;
    size_timers++;
    lastMicros = micros();
    this->updateRPM(newRpm);
}

void SharedTimer::tick(){
    // Serial.print(".");
    // Serial.println(size_timers);
    long now = micros();
    for (int i = 0; i < size_timers; i++) {
        if (instances[i]->rpm > 0.0) {
            instances[i]->doCallback(now);
        } else {
            // Serial.print("ignoring: ");
            // Serial.println(instances[i]->rpm);
        }
    }
}

void SharedTimer::doCallback(long now){
    if (now > nextMicros) {
        // Serial.print("Doing instance: ");
        // Serial.println(rpm);
        lastMicros = nextMicros;
        nextMicros = lastMicros + intervalMicros;
        this->callback();
    } else {
        // Serial.print(this->rpm);
        // Serial.print(',');
        // Serial.print(now);
        // Serial.print(',');
        // Serial.println(nextMicros);
    }
    
}

void SharedTimer::updateRPM(float newRpm) {
    rpm = newRpm;
    intervalMicros = 1000000 / newRpm;
    // Serial.print("interval micros: ");
    // Serial.println(intervalMicros);
    nextMicros = lastMicros + intervalMicros;
}

void setFrequency(uint32_t sampleRate) {

  long start = micros();

  Serial.print(" requested ");
Serial.print(sampleRate);


    uint32_t cc0 = (SystemCoreClock / sampleRate - 1);

    uint32_t prescaler = TC_CTRLA_PRESCALER_DIV1;


    REG_TC4_CTRLA |= prescaler |      // Set prescaler to 1024, 48MHz/1024 = 46.875kHz
                     TC_CTRLA_WAVEGEN_MFRQ |        // Put the timer TC4 into match frequency (MFRQ) mode
                     TC_CTRLA_MODE_COUNT32 |
                     TC_CTRLA_ENABLE ;               // Enable TC4
   while (TC4->COUNT32.STATUS.bit.SYNCBUSY);   
    // doing this last to avoid race
  REG_TC4_COUNT32_CC0 = (uint32_t) cc0;

    
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);       // Wait for synchronization
//    while (GCLK->STATUS.bit.SYNCBUSY);
    REG_TC4_INTFLAG = TC_INTFLAG_OVF;
REG_TC4_INTFLAG |= TC_INTFLAG_OVF;              // Clear the interrupt flags
    REG_TC4_INTENSET = TC_INTENSET_OVF;             // Enable TC4 interrupts
  long end = micros();

Serial.print(" [");
Serial.print(end - start);
Serial.print("] ");

}

void setup_timer(uint32_t sampleRate) {



  
  // Set up the generic clock (GCLK4) used to clock timers
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |          // Divide the 48MHz clock source by divisor 1: 48MHz/1=48MHz
                      GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                       GCLK_GENCTRL_GENEN |         // Enable GCLK4
                       GCLK_GENCTRL_SRC_DFLL48M |   // Set the 48MHz clock source
    GCLK_GENCTRL_ID(4);          // Select GCLK4
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Feed GCLK4 to TC4 and TC5
        REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable GCLK4 to TC4 and TC5
                           GCLK_CLKCTRL_GEN_GCLK4 |     // Select GCLK4
                           GCLK_CLKCTRL_ID_TC4_TC5;     // Feed the GCLK4 to TC4 and TC5
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization



    ; //0xEA;                    // Set the TC4 CC0 register as the TOP value in match frequency mode
    while (TC4->COUNT32.STATUS.bit.SYNCBUSY);       // Wait for synchronization

    NVIC_SetPriority(TC4_IRQn, 0);                  // Set the Nested Vector Interrupt Controller (NVIC) priority for TC4 to 0 (highest)
    NVIC_EnableIRQ(TC4_IRQn);                       // Connect TC4 to Nested Vector Interrupt Controller (NVIC)

    REG_TC4_INTFLAG |= TC_INTFLAG_OVF;              // Clear the interrupt flags
    REG_TC4_INTENSET = TC_INTENSET_OVF;             // Enable TC4 interrupts

    setFrequency(sampleRate);

    // startTime = micros();
    
}

void SharedTimer::begin(){
    Serial.println("Begin: ");
    setup_timer(100);
}




void TC4_Handler()                                // Interrupt Service Routine (ISR) for timer TC4
{     
  // Check for overflow (OVF) interrupt
  if (TC4->COUNT32.INTFLAG.bit.OVF && TC4->COUNT32.INTENSET.bit.OVF){
    // i++;
    // counter++;
    // if(i==256)
    //   i=0;
    REG_TC4_INTFLAG = TC_INTFLAG_OVF;         // Clear the OVF interrupt flag
  }

  SharedTimer::tick();
}