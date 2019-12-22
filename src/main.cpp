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
#include <motor.h>
// #include <AccelStepper.h>
// #include "avdweb_SAMDtimer.h"

float kp = 0.5, ki = 0.0, kd = 0.0;

Sample sample;
Imu imu(sample);
PID pid(kp, ki, kd);
Motor motor(1);

// AccelStepper stepper1(AccelStepper::FULL2WIRE, PIN_STEP, PIN_DIR);
  
volatile int count=0;

float speed = 0.0;

// void ISR_timer3_LED1()
// { static bool b;
//   pinMode(13, OUTPUT);
//   digitalWrite(13, b=!b);
// }


void setup() {


  Serial.begin(1000000);
  while (!Serial);

  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  imu.calibrate(250, 250);
  motor.begin();
  motor.setMicrosteps(4);
  // motor.setFrequency(1);

}



// the loop function runs over and over again forever
void loop() {

  static int16_t count = 0;

  // stepper1.

  // Serial.println(sample.axesGyro.roll);

  if (imu.read()) {
    imu.doCalculations();
    // Serial.print(count);
    // Serial.print(" : ");
    count++;
    speed = pid.calculate(sample.axes.roll, 0.0);
      
    if(count%10 == 0) {
      Serial.print(sample.axes.roll);
      Serial.print(" : ");
      Serial.println((int) speed);
    
    }
    motor.setFrequency((int) speed);
    
    // 
    // Serial.print(sample.axesAccel.roll);
  }
}