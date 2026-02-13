#include "main.h"

#ifndef GR_NEOPIXEL_H
#define GR_NEOPIXEL_H

#include <stdint.h>

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

void Neopixel_update();

// make typedefs for all constants
// create initialization structs (one for GPIO, one for SPI)
// create a function to initialize SPI
// initialize spi in gr_neopixel.c

#endif // GR_NEOPIXEL_H
