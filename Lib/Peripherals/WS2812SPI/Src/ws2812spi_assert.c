#include <assert.h>
#include <stdint.h>

#include "PrivateInc/internal.h"
#include "ws2812spi.h"

static_assert(WS2812SPI_MAX_INSTANCES > 0, "WS2812SPI_MAX_INSTANCES must be greater than 0");
static_assert(WS2812SPI_MAX_INSTANCES <= 6, "WS2812SPI_MAX_INSTANCES cannot exceed six SPI peripherals");

static_assert(WS2812SPI_MAX_LEDS > 0, "WS2812SPI_MAX_LEDS must be greater than 0");
static_assert(WS2812SPI_MAX_LEDS <= UINT16_MAX, "WS2812SPI_MAX_LEDS must fit in a uint16_t");

static_assert(WS2812SPI_MAX_SPI_BITS_PER_PIXEL == 3 || WS2812SPI_MAX_SPI_BITS_PER_PIXEL == 8, "WS2812SPI_MAX_SPI_BITS_PER_PIXEL must be 3 or 8");

static_assert(WS2812SPI_MAX_BUFFER_BYTES > 0, "WS2812SPI_MAX_BUFFER_BYTES must be greater than 0");
static_assert(WS2812SPI_MAX_BUFFER_BYTES <= UINT16_MAX, "WS2812SPI_MAX_BUFFER_BYTES must fit in HAL SPI uint16_t size");
