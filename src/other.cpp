#include "Arduino.h"
#include "sharedtimer.h"

long oneCount = 0;
long twoCount = 0;

void doOne() {
  oneCount ++;
  Serial.print("do one: ");
  Serial.println(oneCount);
  
}

void doTwo() {
  twoCount ++;
  Serial.print("do two: ");
  Serial.println(twoCount);
}

SharedTimer timer1 = SharedTimer(doOne, 0.1);
SharedTimer timer2 = SharedTimer(doTwo, 0.5);

void setup() {

  pinMode(13, OUTPUT);

  Serial.begin(250000);
  while (!Serial);

  Serial.print("instance count: ");
  
  SharedTimer::begin();

Serial.print("one: ");
Serial.println(timer1.intervalMicros);
  

Serial.print("two: ");
Serial.println(timer2.intervalMicros);
  
}

void loop() {

  static bool b;

  delay(5000);
  b=!b;
  // SharedTimer::tick();
  // timer1.updateRPM(0.5);

  delay(6000);
  // timer1.updateRPM(0.1);
  digitalWrite(13, b);

}