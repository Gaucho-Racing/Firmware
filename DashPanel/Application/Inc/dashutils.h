#include <stdint.h>

#include "CANdler.h"
#include "gr_neopixel.h"

#ifndef DASHPANEL_UTILS_H
#define DASHPANEL_UTILS_H

#define NUM_LEDS 3
#define NUM_BUTTONS 2

#define GPIO_POLLING GPIOC
#define PIN_POLLING LL_GPIO_PIN_4
#define DEBOUNCE_TIME 10 // in ms
typedef enum {
	Ready,
	DebouncePress,
	WaitRelease,
	DebounceRelease
} PollingState;

extern uint32_t pollingTimer;

uint32_t MillisecondsSinceBoot(void);
void NeoPixel_Init();
void Neopixel_LEDWrite();
void Neopixel_ButtonWrite();

void PollingStateMachine();

#endif
