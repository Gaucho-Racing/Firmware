#include <stdint.h>

#include "CANdler.h"
#include "gr_neopixel.h"

#ifndef DASHPANEL_UTILS_H
#define DASHPANEL_UTILS_H

#define NUM_LEDS 3
#define NUM_BUTTONS 2

uint32_t MillisecondsSinceBoot(void);
void NeoPixel_Init();
void Neopixel_LEDWrite();
void Neopixel_ButtonWrite();

#endif
