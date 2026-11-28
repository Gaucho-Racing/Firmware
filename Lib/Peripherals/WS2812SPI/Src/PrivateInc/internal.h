#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "spi.h"
#include "ws2812spi.h"

#ifndef WS2812SPI_INTERNAL_H
#define WS2812SPI_INTERNAL_H

#define WS2812SPI_MAX_BUFFER_BYTES ((((uint32_t)WS2812SPI_MAX_LEDS * 24U * WS2812SPI_MAX_SPI_BITS_PER_PIXEL) + 7U) / 8U)

typedef struct WS2812SPI_Private_Handle {
	SPI_HandleTypeDef *spi;
	WS2812SPI_Config config;
	uint16_t encoded_buffer_size;
	uint8_t encoded_buffer[WS2812SPI_MAX_BUFFER_BYTES];
	volatile bool busy;
	bool started;
} WS2812SPI_Private_Handle;

extern WS2812SPI_Private_Handle handles[WS2812SPI_MAX_INSTANCES];

#endif
