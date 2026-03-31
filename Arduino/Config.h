#pragma once

// Recomandare pini GPIO pentru ESP32 (evită pinii 0, 2, 12, 15 la boot)
#define RIGHT_SLIDER_PIN 13
#define RIGHT_SPINNER_PIN 14
#define LEFT_SLIDER_PIN 27
#define LEFT_SPINNER_PIN 26
#define FRONT_SLIDER_PIN 25
#define FRONT_SPINNER_PIN 33
#define BACK_SLIDER_PIN 32
#define BACK_SPINNER_PIN 19 // GPIO19

#define CALIBRATE 0
#define SEQUENCE_BUFFER_SIZE 512 // ESP32 are RAM mult, putem mări bufferul
#define MOVE_BUFFER_SIZE 128