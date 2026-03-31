#pragma once
#include <stdint.h>

enum ServoState
{
    STATE_C,
    STATE_L,
    STATE_R,
    STATE_r,    // 45 grade între CENTER și RIGHT
    STATE_l     // 45 grade între CENTER și LEFT
};

enum ServoType
{   // Par = Spinner, Impar = Slider
    RIGHT_SPINNER,  // 0
    RIGHT_SLIDER,   // 1
    LEFT_SPINNER,   // 2
    LEFT_SLIDER,    // 3
    FRONT_SPINNER,  // 4
    FRONT_SLIDER,   // 5
    BACK_SPINNER,   // 6
    BACK_SLIDER     // 7
};

// Folosim uint32_t pentru a garanta că datele ocupă exact 4 octeți pe ESP32
// și pentru a preveni interpretarea greșită a biților în EEPROM/Flash.
struct ServoCal {
    uint32_t L_us;
    uint32_t R_us;
    uint32_t C_us;
    uint32_t CD_us; // Deviația pentru centrul spinnerelor
};

enum CubeOrientation
{
    ORIENT_NORMAL,
    ORIENT_INVERT
};