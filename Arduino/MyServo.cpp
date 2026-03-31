#include <Arduino.h>
#include <ESP32Servo.h>
#include "MyServo.h"
#include "Config.h"
#include "Calibrate.h"
#include "Types.h"


MyServo::MyServo(int pin, ServoType type, ServoCal calibration) :
    pin(pin),
    type(type),
    cal(calibration)
{
    
    if (static_cast<int>(type) % 2 == 0) {
        pulse = calibration.C_us;
    } else {
        pulse = calibration.R_us;
    }
}

void MyServo::setState(ServoState next) {
    switch (next) {
        case STATE_L: pulse = cal.L_us; break;
        case STATE_R: pulse = cal.R_us; break;
        case STATE_C:
            if (static_cast<int>(type) % 2 == 1) { 
                pulse = cal.C_us;
            } else { 
                if (state == STATE_L || state == STATE_l) pulse = cal.C_us + cal.CD_us;
                else if (state == STATE_R || state == STATE_r) pulse = cal.C_us - cal.CD_us;
                else pulse = cal.C_us;
            }
            break;
        case STATE_r: pulse = (cal.C_us + cal.R_us) / 2; break;
        case STATE_l: pulse = (cal.C_us + cal.L_us) / 2; break;
    }
    state = next;
    if (attached) {
        thisServo.writeMicroseconds(pulse);
    }
}

void MyServo::attach() {
    if (!attached) {
        thisServo.setPeriodHertz(50);
        thisServo.attach(pin, 500, 2500);
        thisServo.writeMicroseconds(pulse);
        attached = true;
    }
}

void MyServo::detach() {
    if (attached) {
        thisServo.detach();
        attached = false;
    }
}

void MyServo::adjustCalibration(int delta) {
    if (state == STATE_C) cal.C_us += delta;
    else if (state == STATE_L) cal.L_us += delta;
    else if (state == STATE_R) cal.R_us += delta;
    pulse += delta;
    if (attached) thisServo.writeMicroseconds(pulse);
}

void MyServo::adjustDeviation(int delta) {
    if (static_cast<int>(type) % 2 == 0) { // Doar pentru Spinners
        if (pulse < cal.C_us) {
            cal.CD_us -= delta;
            if (cal.CD_us < 0) cal.CD_us = 0;
            pulse = cal.C_us - cal.CD_us;
        } else if (pulse > cal.C_us) {
            cal.CD_us += delta;
            pulse = cal.C_us + cal.CD_us;
        }
        if (attached) thisServo.writeMicroseconds(pulse);
    }
}

// DEFINIREA OBIECTELOR (Aici se repară eroarea 'servos was not declared')
MyServo servos[NUM_SERVOS] = {
    [RIGHT_SPINNER] = MyServo(RIGHT_SPINNER_PIN, RIGHT_SPINNER, readCalibration(RIGHT_SPINNER)),
    [RIGHT_SLIDER]  = MyServo(RIGHT_SLIDER_PIN,  RIGHT_SLIDER,  readCalibration(RIGHT_SLIDER)),
    [LEFT_SPINNER]  = MyServo(LEFT_SPINNER_PIN,  LEFT_SPINNER,  readCalibration(LEFT_SPINNER)),
    [LEFT_SLIDER]   = MyServo(LEFT_SLIDER_PIN,   LEFT_SLIDER,   readCalibration(LEFT_SLIDER)),
    [FRONT_SPINNER] = MyServo(FRONT_SPINNER_PIN, FRONT_SPINNER, readCalibration(FRONT_SPINNER)),
    [FRONT_SLIDER]  = MyServo(FRONT_SLIDER_PIN,  FRONT_SLIDER,  readCalibration(FRONT_SLIDER)),
    [BACK_SPINNER]  = MyServo(BACK_SPINNER_PIN,  BACK_SPINNER,  readCalibration(BACK_SPINNER)),
    [BACK_SLIDER]   = MyServo(BACK_SLIDER_PIN,   BACK_SLIDER,   readCalibration(BACK_SLIDER))
};

void attachAllServos() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].attach();
        // MODIFICARE AICI: Am adăugat delay(30) pentru a preveni căderile de tensiune (Brownout) care opresc Bluetooth-ul
        delay(30);
    }
}

void detachAllServos() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].detach();
    }
}

bool parseServoType(char c, ServoType& type) {
    switch (c) {
        case 'R': type = RIGHT_SLIDER; return true;
        case 'L': type = LEFT_SLIDER; return true;
        case 'F': type = FRONT_SLIDER; return true;
        case 'B': type = BACK_SLIDER; return true;
        case 'r': type = RIGHT_SPINNER; return true;
        case 'l': type = LEFT_SPINNER; return true;
        case 'f': type = FRONT_SPINNER; return true;
        case 'b': type = BACK_SPINNER; return true;
        default: return false;
    }
}

void servoTypeToString(ServoType type, char* buffer, size_t bufferSize) {
    const char* names[] = {"R_SPIN", "R_SLID", "L_SPIN", "L_SLID", "F_SPIN", "F_SLID", "B_SPIN", "B_SLID"};
    if (type >= 0 && type < NUM_SERVOS) {
        strncpy(buffer, names[type], bufferSize);
    } else {
        strncpy(buffer, "UNKNOWN", bufferSize);
    }
}