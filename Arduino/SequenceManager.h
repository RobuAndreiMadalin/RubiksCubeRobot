#pragma once
#include "Config.h"
#include "Types.h"

struct SequenceMove
{
    ServoType servo;
    ServoState state;
    unsigned long timeMs;
};

class SequenceManager
{
public:
    SequenceManager() = default;

    char activeSequence[SEQUENCE_BUFFER_SIZE];
    char moveBuf[MOVE_BUFFER_SIZE];

    int startSequence(const char* moveString);
    int startMoves(const char* moveString, int delayMs);
    int tick();
    bool isBusy() const { return busy != 0; }

    unsigned long idleTimeMs;
    CubeOrientation orientation = ORIENT_NORMAL;

private:
    int busy = 0;
    void notifyState();

    int sequenceIndex = 0;
    unsigned long nextMoveAt = 0;
    int executeUntilDelay();
    int handleSequence();

    int movesDelayMs;
    char seqBuf[128];
    int moveIndex = 0;
    int handleMoves();
    void populateActiveSequenceMove(char moveChar);
    void rotateCube(CubeOrientation newOrientation);
};

extern SequenceManager seqManager;