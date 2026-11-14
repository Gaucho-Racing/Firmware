#include <stdint.h>

#include "main.h"

#ifndef EVENTS_H
#define EVENTS_H

typedef struct {
	uint32_t timestamp_ms;
	GPIO_PinState pin_state;
} PinEvent;

void events_add_event(PinEvent event);

#endif
