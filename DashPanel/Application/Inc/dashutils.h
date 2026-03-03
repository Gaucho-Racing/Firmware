#include <stdint.h>
#include "gr_neopixel.h"
#include "CANdler.h"

#ifndef DASHPANEL_UTILS_H
#define DASHPANEL_UTILS_H

#define NUM_LEDS 3
#define NUM_BUTTONS 2

static Neopixel_Color LED_colors[NUM_LEDS];
static Neopixel_Color button_colors[NUM_BUTTONS];

uint32_t MillisecondsSinceBoot(void);
void Neopixel_LEDWrite();
void Neopixel_ButtonWrite();

#endif // DASHPANEL_UTILS_H
