/**
   What is this file?
   This is an arduino example that calculates accurate roll,pitch,yaw from raw gyro/accelerometer data
   It has a calibration stage which removes most of the gyro drift and a complementary filter
   that combines gyro and accelerometer angles to produce roll/pitch angles that don't drift (like the gyro angle) and aren't noisy
   (like the accel angles). As there is no magnetic compass on the nano iot 33, it's not possible to 'complement' the yaw
   - hence yaw will drift and is 'relative'.
*/
#include <Arduino_LSM6DS3.h>
#include <Wire.h>
#include <imu.h>
#include <pid.h>
#include <stepper.h>

#define PIN_ENABLE 4 
#define PIN_M0 5
#define PIN_M1 6
#define PIN_M2 7
#define PIN_RESET 8
#define PIN_SLEEP 9
#define PIN_STEP 10
#define PIN_DIR 11

float kp = 0.5, ki = 0.0, kd = 0.0;

Sample sample;
Imu imu(sample);
PID pid(kp, ki, kd);
Stepper stepper1(10, 11);
  
volatile int count=0;

float speed = 0.0;

void setup() {

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


  Serial.begin(1000000);
  while (!Serial);

  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  imu.calibrate(250, 250);
  stepper1.begin();
  stepper1.setMicrosteps(4, PIN_M0, PIN_M1, PIN_M2);
  // motor.setFrequency(1);

}



// the loop function runs over and over again forever
void loop() {

  static bool b;

  static int16_t count = 0;

  if (imu.read()) {
    imu.doCalculations();
    // Serial.print(count);
    // Serial.print(" : ");
    count++;
    speed = pid.calculate(sample.axes.roll, 0.0);
    int period = stepper1.setFrequency(speed);
      
    if(count%10 == 0) {
      imu.printCalculations();
      // Serial.print(sample.axes.roll);
      // Serial.print(",");
      // Serial.print(sample.axesAccel.roll);
      // Serial.print(",");
      // Serial.print(sample.axesGyro.roll);
      // Serial.print(",");
      // Serial.print(sample.interval);
      // Serial.print(" : ");
      // Serial.print(speed);
      // Serial.print(" : ");
      // Serial.print(period);
      Serial.println("");
    if(count%100 == 0) {
      
      digitalWrite(PIN_LED, b = !b);
    }
    }
    
  }
}