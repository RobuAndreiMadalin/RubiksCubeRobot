#include "SequenceManager.h"
#include "MyServo.h"
#include <ctype.h> 
#include <stdlib.h>
#include <Arduino.h>

SequenceManager seqManager;

static bool parseServoState(char c, ServoState& state)
{
    switch (c)
    {
        case 'L': state = STATE_L; return true;
        case 'C': state = STATE_C; return true;
        case 'R': state = STATE_R; return true;
        case 'r': state = STATE_r; return true;
        case 'l': state = STATE_l; return true;
        default: return false;
    }
}

int SequenceManager::startSequence(const char* moveString)
{
    if (!moveString || *moveString == '\0')
        return -2;

    if (moveString[0] == 'C' && moveString[1] == '\0')
    {
        busy = 0;
        idleTimeMs = millis();
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    if (busy)
        return -1;

    strncpy(activeSequence, moveString, sizeof(activeSequence) - 1);
    activeSequence[sizeof(activeSequence) - 1] = '\0';

    sequenceIndex = 0;
    nextMoveAt = millis() + 100;    
    busy = 1;   
    notifyState();

    return 0;
}

int SequenceManager::startMoves(const char* moveString, int delayMs)
{
    if (!moveString || *moveString == '\0')
        return -2;

    if (busy)
        return -1;

    movesDelayMs = delayMs;
    strncpy(moveBuf, moveString, sizeof(moveBuf) - 1);
    activeSequence[sizeof(moveBuf) - 1] = '\0'; 

    sequenceIndex = 0;
    nextMoveAt = millis() + 100;    
    busy = 2;   
    notifyState();

    return 0;
}

int SequenceManager::tick()
{
    if (!isBusy())
        return 0;

    unsigned long now = millis();
    if (now < nextMoveAt)
        return 0;

    return executeUntilDelay();
}

void SequenceManager::notifyState()
{
    if (isBusy())
        Serial.println("BUSY");
    else
        Serial.println("IDLE");
}

int SequenceManager::executeUntilDelay()
{
    if (busy == 1)
        return handleSequence();
    else if (busy == 2)
        return handleMoves();

    return -10;  
}

int SequenceManager::handleSequence()
{
    while (activeSequence[sequenceIndex] != '\0' &&
       !isdigit(activeSequence[sequenceIndex]))
    {
        ServoType servoType;
        if (!parseServoType(activeSequence[sequenceIndex], servoType))
        {
            busy = 0;
            idleTimeMs = millis();
            activeSequence[0] = '\0';
            notifyState();
            return -3;  
        }
        sequenceIndex++;

        ServoState state;
        if (!parseServoState(activeSequence[sequenceIndex], state))
        {
            busy = 0;
            idleTimeMs = millis();
            activeSequence[0] = '\0';
            notifyState();
            return -4;  
        }
        sequenceIndex++;

        servos[servoType].setState(state);
    }

    if (activeSequence[sequenceIndex] == '\0')
    {
        if (busy == 2)
            return 2; 

        busy = 0;
        idleTimeMs = millis();
        activeSequence[0] = '\0';
        notifyState();
        return 0;
    }

    if (isdigit(activeSequence[sequenceIndex]))
    {
        char* endPtr;
        unsigned long delay =
            strtoul(&activeSequence[sequenceIndex], &endPtr, 10);

        sequenceIndex = endPtr - activeSequence;
        nextMoveAt = millis() + delay;
        return 0;
    }

    return 1;   
}

int SequenceManager::handleMoves()
{
    int seqResult = handleSequence();
    if (seqResult < 0)
    {
        busy = 0;   
        idleTimeMs = millis();
        activeSequence[0] = '\0';
        moveBuf[0] = '\0';
        notifyState();
        return seqResult;
    }
    if (seqResult != 2)
    {
        return 0;   
    }

    char moveChar = moveBuf[moveIndex];
    if (moveChar == '\0')
    {
        busy = 0;   
        idleTimeMs = millis();
        activeSequence[0] = '\0';
        moveBuf[0] = '\0';
        moveIndex = 0;
        sequenceIndex = 0;
        notifyState();
        return 0;
    }

    activeSequence[0] = '\0';
    populateActiveSequenceMove(moveChar);
    sequenceIndex = 0;
    nextMoveAt = millis();
    moveIndex++;
    return 1;
}

void SequenceManager::rotateCube(CubeOrientation newOrientation)
{
    if (orientation == newOrientation)
        return;

    char *p = activeSequence;
    p += strlen(p);
    sprintf(p, "RLLL");
    p += strlen(p);
    sprintf(p, "FRBR%d", movesDelayMs);
    p += strlen(p);
    if (newOrientation == ORIENT_INVERT)
        sprintf(p, "fRbL%d", movesDelayMs);
    else
        sprintf(p, "fLbR%d", movesDelayMs);
    p += strlen(p);
    sprintf(p, "FLBL%d", movesDelayMs);
    p += strlen(p);
    sprintf(p, "RRLR%d", movesDelayMs);
    p += strlen(p);
    sprintf(p, "fCfCbCbC%d", movesDelayMs);
    p += strlen(p);
    sprintf(p, "RCLC%d", movesDelayMs);
    p += strlen(p);
    sprintf(p, "FCBC");

    orientation = newOrientation;
}

void SequenceManager::populateActiveSequenceMove(char moveChar)
{
    char *p = activeSequence;

    switch (moveChar)
    {
    case 'U':
        rotateCube(ORIENT_INVERT);
        p += strlen(p);
        sprintf(p, "lR%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);
        break;

    case 'u':
        rotateCube(ORIENT_INVERT);
        p += strlen(p);
        sprintf(p, "lL%d", movesDelayMs); 
        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs); 
        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs); 
        break;

    case 'D':
        rotateCube(ORIENT_INVERT);
        p += strlen(p);
        sprintf(p, "rR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'd':
        rotateCube(ORIENT_INVERT);
        p += strlen(p);
        sprintf(p, "rL%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'L':
        rotateCube(ORIENT_NORMAL);
        p += strlen(p);
        sprintf(p, "lR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);  
        break;

    case 'l':
        rotateCube(ORIENT_NORMAL);
        p += strlen(p);
        sprintf(p, "lL%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "LR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "lClC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "LC%d", movesDelayMs);  
        break;

    case 'R':
        rotateCube(ORIENT_NORMAL);
        p += strlen(p);
        sprintf(p, "rR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'r':
        rotateCube(ORIENT_NORMAL);
        p += strlen(p);
        sprintf(p, "rL%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "RR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "rCrC%d", movesDelayMs);
        p += strlen(p);
        sprintf(p, "RC%d", movesDelayMs);  
        break;

    case 'F':
        p += strlen(p);
        sprintf(p, "fR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "FR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "fCfC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "FC%d", movesDelayMs);  
        break;

    case 'f':
        p += strlen(p);
        sprintf(p, "fL%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "FR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "fCfC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "FC%d", movesDelayMs);  
        break;

    case 'B':
        p += strlen(p);
        sprintf(p, "bR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "BR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "bCbC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "BC%d", movesDelayMs);  
        break;

    case 'b':
        p += strlen(p);
        sprintf(p, "bL%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "BR%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "bCbC%d", movesDelayMs);  
        p += strlen(p);
        sprintf(p, "BC%d", movesDelayMs);  
        break;

    default:
        break;
    }
}