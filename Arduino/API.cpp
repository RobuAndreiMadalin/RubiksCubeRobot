#include <Arduino.h>
#include "BluetoothSerial.h"
#include "MyServo.h"
#include "SequenceManager.h"
#include "Config.h"
#include "API.h"
#include "Types.h"

#define MAX_TOKENS 5

BluetoothSerial SerialBT;

static char rxBuf[SEQUENCE_BUFFER_SIZE];
static char* tokens[MAX_TOKENS];

void APISetup() {
    if (!SerialBT.begin("RubikESP")) {
        Serial.println("Eroare la pornire Bluetooth!");
    } else {
        Serial.println("Bluetooth pornit! Nume: RubikESP");
    }
    SerialBT.setTimeout(50);
}

static int tokenize(char* line, char** tokens, int maxTokens) {
    int count = 0;
    while (*line && count < maxTokens) {
        while (*line == ' ') ++line;
        if (*line == '\0') break;
        tokens[count++] = line;
        while (*line && *line != ' ') ++line;
        if (*line) {
            *line = '\0';
            ++line;
        }
    }
    return count;
}

void APILoop() {
    if (!SerialBT.available() && !Serial.available()) return;

    int len = 0;
    if (SerialBT.available()) {
        len = SerialBT.readBytesUntil('\n', rxBuf, SEQUENCE_BUFFER_SIZE - 1);
    } else {
        len = Serial.readBytesUntil('\n', rxBuf, SEQUENCE_BUFFER_SIZE - 1);
    }

    rxBuf[len] = '\0';
    if (len > 0 && rxBuf[len - 1] == '\r') rxBuf[len - 1] = '\0';
    if (rxBuf[0] == '\0') return;

    int tokenCount = tokenize(rxBuf, tokens, MAX_TOKENS);
    if (tokenCount == 0) return;

    const char* cmd = tokens[0];

    if (strcmp(cmd, "PING") == 0) {
        SerialBT.println("PONG");
        Serial.println("PONG");
        return;
    }

    if (strcmp(cmd, "HELP") == 0) {
        SerialBT.println("Commands: HELP, PING, STATUS, SEQ, MOVE");
        return;
    }

    if (strcmp(cmd, "SEQ") == 0) {
        if (tokenCount != 2) { SerialBT.println("ERR args"); return; }
        int res = seqManager.startSequence(tokens[1]);
        if (res == 0) SerialBT.println("OK");
        else SerialBT.println("ERR");
        return;
    }

    if (strcmp(cmd, "MOVE") == 0) {
        if (tokenCount != 4) { SerialBT.println("ERR args"); return; }
        int delay = atoi(tokens[1]);
        int inputOrient = atoi(tokens[2]);
        seqManager.orientation = (inputOrient == 0) ? ORIENT_NORMAL : ORIENT_INVERT;
        int res = seqManager.startMoves(tokens[3], delay);
        if (res == 0) SerialBT.println("OK");
        else SerialBT.println("ERR");
        return;
    }

    if (strcmp(cmd, "STATUS") == 0) {
        SerialBT.println(seqManager.isBusy() ? "BUSY" : "IDLE");
        return;
    }
}