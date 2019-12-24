#include "Arduino.h"
#include <Arduino_LSM6DS3.h>
#include <imu.h>


Imu::Imu(Sample &sample)
{
  _sample = &sample;
  _lastTime = micros();
}

void Imu::calibrate(unsigned int delayMillis, unsigned int calibrationMillis) {
    Serial.println("imu calibrate");
    int calibrationCount = 0;

  delay(delayMillis); // to avoid shakes after pressing reset button

  float sumX = 0.0, 
        sumY = 0.0, 
        sumZ = 0.0;

  uint startTime = millis();
  while (millis() < startTime + calibrationMillis) {
    if (read()) {
      // in an ideal world coordGyro.x/Y/Z == 0, anything higher or lower represents drift
      sumX += _sample->coordGyro.x;
      sumY += _sample->coordGyro.y;
      sumZ += _sample->coordGyro.z;

      calibrationCount++;
    }
  }

  if (calibrationCount == 0) {
    Serial.println("Failed to calibrate");
  }

  _sample->coordGyroDrift.x = sumX / calibrationCount;
  _sample->coordGyroDrift.y = sumY / calibrationCount;
  _sample->coordGyroDrift.z = sumZ / calibrationCount;

  // now make sure the initial angle is sensible
  read();
  doCalculations();
  _sample->axes.roll = _sample->axesAccel.roll;
  _sample->axes.pitch = _sample->axesAccel.pitch;
  _sample->axesGyro.roll = _sample->axesAccel.roll;
  _sample->axesGyro.pitch = _sample->axesAccel.pitch;

  Serial.println(_sample->coordGyroDrift.z);

}

boolean Imu::read() {

  
  // Serial.println("is ready");
  if (IMU.accelerationAvailable() || IMU.gyroscopeAvailable() ) {
    long currentTime = micros();
    _lastInterval = currentTime - _lastTime; // expecting this to be ~104Hz +- 4%
    _lastTime = currentTime;

    _sample->interval = _lastInterval;
    _sample->frequency = _lastInterval;

    IMU.readAcceleration(_sample->coordAccel.x, _sample->coordAccel.y, _sample->coordAccel.z);
    IMU.readGyroscope(_sample->coordGyro.x, _sample->coordGyro.y, _sample->coordGyro.z);
    // doCalculations();
    // Serial.println(_sample->coordGyro.z);
    //  Serial.println(_sample->axes.roll);
    return true;
  }
  return false;
}

void Imu::doCalculations() {
  _sample->axesAccel.roll = atan2(_sample->coordAccel.y, _sample->coordAccel.z) * 180 / M_PI;
  _sample->axesAccel.pitch = atan2(-_sample->coordAccel.x, sqrt(_sample->coordAccel.y * _sample->coordAccel.y + _sample->coordAccel.z * _sample->coordAccel.z)) * 180 / M_PI;

  _sample->frequency = 1000000 / _lastInterval;

  _sample->axesGyro.roll = _sample->axesGyro.roll + (_sample->coordGyro.x / _sample->frequency);
  _sample->axesGyro.pitch = _sample->axesGyro.pitch + (_sample->coordGyro.y / _sample->frequency);
  _sample->axesGyro.yaw = _sample->axesGyro.yaw + (_sample->coordGyro.z / _sample->frequency);

  _sample->axesGyroCalibrated.roll = _sample->axesGyroCalibrated.roll + ((_sample->coordGyro.x - _sample->coordGyroDrift.x) / _sample->frequency);
  _sample->axesGyroCalibrated.pitch = _sample->axesGyroCalibrated.pitch + ((_sample->coordGyro.y - _sample->coordGyroDrift.y) / _sample->frequency);
  _sample->axesGyroCalibrated.yaw = _sample->axesGyroCalibrated.yaw + ((_sample->coordGyro.z - _sample->coordGyroDrift.z) / _sample->frequency);

  _sample->axes.roll = _sample->axes.roll + ((_sample->coordGyro.x - _sample->coordGyroDrift.x) / _sample->frequency);
  _sample->axes.pitch = _sample->axes.pitch + ((_sample->coordGyro.y - _sample->coordGyroDrift.y) / _sample->frequency);
  _sample->axes.yaw = _sample->axes.yaw + ((_sample->coordGyro.z - _sample->coordGyroDrift.z) / _sample->frequency);

  _sample->axes.roll = 0.98 * _sample->axes.roll + 0.02 * _sample->axesAccel.roll;
  _sample->axes.pitch = 0.98 * _sample->axes.pitch + 0.02 * _sample->axesAccel.pitch;
}

void Imu::printCalculations() {
  Serial.print(_sample->axesGyro.roll);
  Serial.print(',');
  Serial.print(_sample->axesGyro.roll);
  Serial.print(',');
  Serial.print(_sample->axesGyro.yaw);
  Serial.print(',');
  Serial.print(_sample->axesGyroCalibrated.roll);
  Serial.print(',');
  Serial.print(_sample->axesGyroCalibrated.pitch);
  Serial.print(',');
  Serial.print(_sample->axesGyroCalibrated.yaw);
  Serial.print(',');
  Serial.print(_sample->axesAccel.roll);
  Serial.print(',');
  Serial.print(_sample->axesAccel.pitch);
  Serial.print(',');
  Serial.print(_sample->axesAccel.yaw);
  Serial.print(',');
  Serial.print(_sample->axes.roll);
  Serial.print(',');
  Serial.print(_sample->axes.pitch);
  Serial.print(',');
  Serial.print(_sample->axesAccel.yaw);
  Serial.print(',');
  Serial.print(_sample->interval);
  Serial.print(',');
  Serial.print(_sample->frequency);
  Serial.println("");
}