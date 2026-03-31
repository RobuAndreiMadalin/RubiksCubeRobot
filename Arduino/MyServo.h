#pragma once
#include <Arduino.h>
#include "Config.h"
#include "Types.h"
#include <ESP32Servo.h> 


void servoTypeToString(ServoType type, char* buffer, size_t bufferSize);


bool parseServoType(char c, ServoType& type);

class MyServo
{
private:
    int pin{};
    Servo thisServo{};
    ServoType type{};
    ServoCal cal{};
    ServoState state{STATE_C};
    int pulse{};    
    bool attached{false};

public:

    
    MyServo(int pin, ServoType type, ServoCal calibration);

    
    void setState(ServoState next);

    ServoState getState() const
    {
        return state;
    }

    int pulseWidth() const
    {
        return pulse;
    }

    int getPin() const
    {
        return pin;
    }

    ServoType getType() const
    {
        return type;
    }

    ServoCal getCalibration() const
    {
        return cal;
    }

    void adjustCalibration(int delta);

    void adjustDeviation(int delta);

    void attach();

    void detach();
};

#define NUM_SERVOS 8

extern MyServo servos[NUM_SERVOS];

void attachAllServos();

void detachAllServos();
