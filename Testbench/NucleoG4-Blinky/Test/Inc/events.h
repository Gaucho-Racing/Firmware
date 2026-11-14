#include "main.h"
#include <stdint.h>

#ifndef EVENTS_H
#define EVENTS_H

typedef struct {
	GPIO_PinState pin_state;
	uint32_t timestamp_ms;
} PinEvent;

#endif
