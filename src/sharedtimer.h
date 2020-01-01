#ifndef shared_timer_h
#define shared_timer_h
#include "Arduino.h"

#define MAX_INSTANCES 4

using functionPtr = void(*)(void);

class SharedTimer
{
    public:
    SharedTimer(functionPtr action, float rpm);

    static void begin();
    // static int _timerCount;    
    static void tick();
    void updateRPM(float rpm);
    long intervalMicros;
    private:
    void doCallback(long now);
    float rpm;
    
    long lastMicros;
    long nextMicros;
    static SharedTimer* instances[MAX_INSTANCES];
    static int size_timers;
    functionPtr callback;
    
    

};
#endif