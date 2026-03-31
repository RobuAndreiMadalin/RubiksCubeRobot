#include <Arduino.h>
#include <EEPROM.h>
#include "MyServo.h"
#include "Calibrate.h"
#include "Config.h"

#define EEPROM_SIZE 512 
#define EEPROM_MAGIC 0xCAFE
#define EEPROM_MAGIC_ADDR 100
#define EEPROM_CALIBRATION_START_ADDR 102

static int selectedServo = 0;
static ServoState mode = STATE_C;
static const int step = 5;

void resetEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) { 
        Serial.println("Error initializing EEPROM!");
        return;
    }

    uint16_t magic;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);

    if (magic != EEPROM_MAGIC) {
        Serial.println("!!! Corrupted memory detected. Resetting to 1000/1500/2000...");
        for (int i = 0; i < NUM_SERVOS; i++) {
            ServoCal initCal = {1000, 2000, 1500, 0}; 
            EEPROM.put(EEPROM_CALIBRATION_START_ADDR + (i * sizeof(ServoCal)), initCal);
        }
        EEPROM.put(EEPROM_MAGIC_ADDR, (uint16_t)EEPROM_MAGIC);
        EEPROM.commit();
        Serial.println("EEPROM successfully repaired!");
        
        for (int i = 0; i < NUM_SERVOS; i++) {
            ServoCal freshCal = readCalibration((ServoType)i);
        }
    }
}

ServoCal readCalibration(ServoType type) {
    ServoCal cal;
    
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(EEPROM_CALIBRATION_START_ADDR + (int)type * sizeof(ServoCal), cal);
    
    if (cal.C_us > 3000 || cal.C_us < 500) {
        cal = {1000, 2000, 1500, 0};
    }
    return cal;
}

void writeCalibration(ServoType type, ServoCal cal) {
    EEPROM.put(EEPROM_CALIBRATION_START_ADDR + (int)type * sizeof(ServoCal), cal);
    EEPROM.commit();
}

void writeAllCalibrations() {
    Serial.println("Saving to Flash...");
    for (int i = 0; i < NUM_SERVOS; i++) {
        ServoCal cal = servos[i].getCalibration();
        EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(ServoCal), cal);
    }
    if(EEPROM.commit()) {
        Serial.println("SAVED! Now set CALIBRATE to false.");
    } else {
        Serial.println("SAVE ERROR!");
    }
}

void printStatus() {
    Serial.print("Servo: "); Serial.print(selectedServo);
    char buffer[20];
    servoTypeToString(servos[selectedServo].getType(), buffer, sizeof(buffer));
    Serial.print(" ("); Serial.print(buffer); Serial.print(")");
    
    ServoCal cal = servos[selectedServo].getCalibration();
    Serial.print(" | Mode: ");
    if(mode == STATE_C) Serial.print("CENTER");
    else if(mode == STATE_L) Serial.print("LEFT");
    else Serial.print("RIGHT");
    
    Serial.printf(" | Pulse: %d us\n", servos[selectedServo].pulseWidth());
}

void printCalibrations() {
    Serial.println("\n--- CURRENT CALIBRATION VALUES (FROM RAM) ---");
    for (int i = 0; i < NUM_SERVOS; i++) {
        ServoCal c = servos[i].getCalibration();
        Serial.printf("S%d | L:%u | R:%u | C:%u | Dev:%u\n", i, c.L_us, c.R_us, c.C_us, c.CD_us);
    }
    Serial.println("-------------------------------------------\n");
}

void calibrateSetup() {
    if (!EEPROM.begin(EEPROM_SIZE)) Serial.println("EEPROM init failed");
    
    resetEEPROM();
    
#if CALIBRATE
    for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].setState(STATE_C);
    }
    Serial.println("System ready for calibration.");
    printStatus();
#endif
}

void calibrateLoop() {
    if (!Serial.available()) return;
    char c = Serial.read();

    switch(c) {
        case '0' ... '7': selectedServo = c - '0'; printStatus(); break;
        case 'c': mode = STATE_C; servos[selectedServo].setState(STATE_C); printStatus(); break;
        case 'l': mode = STATE_L; servos[selectedServo].setState(STATE_L); printStatus(); break;
        case 'r': mode = STATE_R; servos[selectedServo].setState(STATE_R); printStatus(); break;
        case '+': servos[selectedServo].adjustCalibration(step); printStatus(); break;
        case '-': servos[selectedServo].adjustCalibration(-step); printStatus(); break;
        case '*': servos[selectedServo].adjustCalibration(step * 10); printStatus(); break;
        case '/': servos[selectedServo].adjustCalibration(-step * 10); printStatus(); break;
        case '>': servos[selectedServo].adjustDeviation(step); printStatus(); break;
        case '<': servos[selectedServo].adjustDeviation(-step); printStatus(); break;
        case 'p': printCalibrations(); break;
        case 'w': writeAllCalibrations(); break;
        case 'h': 
            Serial.println("Commands: 0-7, c/l/r, +/-, p(print), w(save)");
            break;
    }
}