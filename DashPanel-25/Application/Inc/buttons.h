#include <stdbool.h>

#include "dash.h"

#ifndef BUTTONS_H
    #define BUTTONS_H

    #define POLL_BUTTON_STATE_DELAY 20
    #define UPDATE_BUTTON_COLORS_DELAY 150

    typedef enum {
        RTD_BUTTON = 1,
        TS_ACTIVE_BUTTON = 2,
    } ButtonNames;
    // Must be 1 or 2 for LED1 or LED2 respectively

    // Format is GRB, dark tech, must be kept at 24 bits (byte per color)
    typedef enum {
        COLOR_GREEN = (uint32_t)0xFF0000,
        COLOR_RED = (uint32_t)0x03FC00,
        COLOR_BLUE = (uint32_t)0x0000FF,
    } Color;

    typedef union {
        struct {
            Color TS_Active;
            Color RTD;
        };

        uint32_t rawData[2];
    } NeoPixelData;

    void pollButtonState(void* args);
    void updateButtonColors(void* args);
#endif
