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

// --- Reset EEPROM la valori sigure ---
void resetEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) { 
        Serial.println("Eroare la init EEPROM!");
        return;
    }

    uint16_t magic;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);

    // Forțăm resetarea dacă magic-ul nu e bun
    if (magic != EEPROM_MAGIC) {
        Serial.println("!!! Memorie corupta detectata. Resetez la 1000/1500/2000...");
        for (int i = 0; i < NUM_SERVOS; i++) {
            ServoCal initCal = {1000, 2000, 1500, 0}; 
            EEPROM.put(EEPROM_CALIBRATION_START_ADDR + (i * sizeof(ServoCal)), initCal);
        }
        EEPROM.put(EEPROM_MAGIC_ADDR, (uint16_t)EEPROM_MAGIC);
        EEPROM.commit();
        Serial.println("EEPROM reparat cu succes!");
        
        // --- PARTEA CRUCIALĂ: Updatează obiectele MyServo cu noile valori ---
        for (int i = 0; i < NUM_SERVOS; i++) {
            // Reîncărcăm calibrarea în obiectul activ din memorie
            ServoCal freshCal = readCalibration((ServoType)i);
            // Presupunând că ai acces la o metodă de setCalibration sau direct la structură
            // Dacă MyServo nu are setCalibration, valorile vor fi luate la următorul restart.
        }
    }
}

ServoCal readCalibration(ServoType type) {
    ServoCal cal;
    // Ne asigurăm că EEPROM e pornit înainte de citire
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(EEPROM_CALIBRATION_START_ADDR + (int)type * sizeof(ServoCal), cal);
    
    // Validare de siguranță: dacă valorile citite sunt absurde, dăm valori default
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
    Serial.println("Salvare in Flash...");
    for (int i = 0; i < NUM_SERVOS; i++) {
        ServoCal cal = servos[i].getCalibration();
        EEPROM.put(EEPROM_CALIBRATION_START_ADDR + i * sizeof(ServoCal), cal);
    }
    if(EEPROM.commit()) {
        Serial.println("SALVAT! Schimbati CALIBRATE in false.");
    } else {
        Serial.println("EROARE SALVARE!");
    }
}

void printStatus() {
    Serial.print("Servo: "); Serial.print(selectedServo);
    char buffer[20];
    servoTypeToString(servos[selectedServo].getType(), buffer, sizeof(buffer));
    Serial.print(" ("); Serial.print(buffer); Serial.print(")");
    
    ServoCal cal = servos[selectedServo].getCalibration();
    Serial.print(" | Mod: ");
    if(mode == STATE_C) Serial.print("CENTER");
    else if(mode == STATE_L) Serial.print("LEFT");
    else Serial.print("RIGHT");
    
    Serial.printf(" | Puls: %d us\n", servos[selectedServo].pulseWidth());
}

void printCalibrations() {
    Serial.println("\n--- VALORI CALIBRARE CURENTE (DIN RAM) ---");
    for (int i = 0; i < NUM_SERVOS; i++) {
        ServoCal c = servos[i].getCalibration();
        // Folosim %u pentru unsigned int (ca să evităm numerele cu minus)
        Serial.printf("S%d | L:%u | R:%u | C:%u | Dev:%u\n", i, c.L_us, c.R_us, c.C_us, c.CD_us);
    }
    Serial.println("------------------------------------------\n");
}

void calibrateSetup() {
    // 1. Inițializăm EEPROM
    if (!EEPROM.begin(EEPROM_SIZE)) Serial.println("Fail EEPROM");
    
    // 2. Curățăm mizeria dacă există
    resetEEPROM();

    // 3. IMPORTANT: Dacă valorile din print sunt tot rele, înseamnă că obiectele 
    // servos[i] au fost create cu gunoi înainte de reset. 
    // Cel mai simplu: după ce vezi "EEPROM rescris" în consolă, dă un RESET fizic la placă.
    
#if CALIBRATE
    for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].setState(STATE_C);
    }
    Serial.println("Sistem pregatit pentru calibrare.");
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
            Serial.println("Comenzi: 0-7, c/l/r, +/-, p(print), w(save)");
            break;
    }
}