#include <Arduino.h>
#include <ESP32Servo.h>
#include <EEPROM.h>
#include "Calibrate.h"
#include "MyServo.h"
#include "SequenceManager.h"
#include "API.h"

bool servosAreAttached = false;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(3000);

    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

#if CALIBRATE
    if (!EEPROM.begin(512)) {
        Serial.println("EEPROM init failed!");
    } else {
        Serial.println("Resetez EEPROM pentru calibrare...");
        for (int i = 0; i < NUM_SERVOS; i++) {
            ServoCal initCal = {1000, 2000, 1500, 0};
            EEPROM.put(102 + i * sizeof(ServoCal), initCal);
        }
        EEPROM.put(100, 0xCAFE);
        EEPROM.commit();
        Serial.println("EEPROM resetat complet!");
    }
    calibrateSetup();
#else
    APISetup();
#endif
}

void loop() {
    bool shouldBeActive = (millis() - seqManager.idleTimeMs < 3000 || seqManager.isBusy() || CALIBRATE);

    if (shouldBeActive && !servosAreAttached) {
        attachAllServos();
        servosAreAttached = true;
        Serial.println("Servos: ATTACHED");
    }
    else if (!shouldBeActive && servosAreAttached) {
        detachAllServos();
        servosAreAttached = false;
        Serial.println("Servos: DETACHED (Power Save)");
    }

#if CALIBRATE
    calibrateLoop();
#else
    APILoop();
    int res = seqManager.tick();
    if (res < 0) {
        Serial.print("SEQ ERR ");
        Serial.println(res);
    }
#endif

    delay(1);
}