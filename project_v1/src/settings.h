#ifndef SETTINGS_H
#define SETTINGS_H

#include "application.h"

/*
// --- Cloud functions

  selectWifi(input);
    input:
      nbxyzmobile

  controlSD(command);
    command:
      attach
      eject
      status

// --- Cloud variables

*/

// --- Input and outputs ---

// Analog
#define LIGHTSENSOR A0
//#define ??? A1
#define SD_CS A2
#define SD_SCK A3
#define SD_MISO A4
#define SD_MOSI A5

// Digital
#define RGBRED D0
#define RGBGREEN D1
#define RGBBLUE D2
// #define ??? D3
// #define ??? D4
// #define ??? D5
// #define ??? D6
#define SDLED D7

// DAC
// #define ??? DAC

#endif /* end of include guard: SETTINGS_H */
