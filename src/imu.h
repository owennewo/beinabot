#ifndef imu_h
#define imu_h
#include "Arduino.h"

struct Coord {
    float x, y, z;
};

struct Axes {
    float roll, pitch, yaw;
};

struct Sample {

  Coord coordAccel; // units m/s/s i.e. accelZ if often 9.8 (gravity)
  Coord coordGyro; // units dps (degrees per second)
  Coord coordGyroDrift; // units dps
  Axes axesAccel; // units degrees (roll and pitch noisy, yaw not possible)
  Axes axesGyro; // units degrees (expect major drift)
  Axes axesGyroCalibrated; // units degrees (expect minor drift)
  Axes axes; // units degrees (excellent roll, pitch, yaw minor drift)
  long interval;
  float frequency;

};

class Imu
{
  public:
    Imu(Sample &sample);
    void calibrate(unsigned int delayMillis, unsigned int calibrationMillis);
    // Sample read();
    boolean read();
    void doCalculations();
    void printCalculations();
  private:
    int _pin;
    Sample *_sample;
    long _lastInterval;
    long _lastTime;
};



#endif