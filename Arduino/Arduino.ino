#include <Arduino.h>
#include <ESP32Servo.h>
#include <EEPROM.h>
#include "Calibrate.h"
#include "MyServo.h"
#include "SequenceManager.h"
#include "API.h"

// Variabilă globală pentru a urmări starea servourilor
bool servosAreAttached = false;

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(3000);

    // Am lăsat timerele 1, 2 și 3 pentru servouri
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

#if CALIBRATE
    // REINITIALIZE EEPROM COMPLET (Doar in modul calibrare)
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
    // AICI ERA PROBLEMA! Bluetooth-ul nu era inițializat niciodată.
    // Această funcție pornește semnalul Bluetooth:
    APISetup();
#endif
}

void loop() {
    // 1. Calculăm dacă AR TREBUI să fie pornite servourile
    bool shouldBeActive = (millis() - seqManager.idleTimeMs < 3000 || seqManager.isBusy() || CALIBRATE);

    // 2. Facem attach/detach DOAR când se schimbă starea (Esențial pentru stabilitate)
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
    // 3. Permite stack-ului Bluetooth să proceseze datele
    APILoop();
   
    int res = seqManager.tick();
    if (res < 0) {
        Serial.print("SEQ ERR ");
        Serial.println(res);
    }
#endif

    // Un mic delay ajută stabilitatea stack-ului de rețea/BT pe ESP32
    delay(1);
}