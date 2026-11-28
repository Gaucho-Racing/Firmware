#include <stdint.h>

#include "main.h"
#include "spi.h"

#ifndef WS2812SPI_H
#define WS2812SPI_H

typedef struct WS2812SPI_Private_Handle WS2812SPI_Handle;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} WS2812SPI_Color;

typedef enum {
	WS2812SPI_ORDER_RGB = 0,
	WS2812SPI_ORDER_GRB = 1,
} WS2812SPI_ColorOrder;

typedef struct {
	WS2812SPI_ColorOrder color_order;
	uint16_t led_count;
	uint8_t spi_bits_per_pixel;
} WS2812SPI_Config;

#define WS2812SPI_COLOR_RED ((WS2812SPI_Color){.red = 255, .green = 0, .blue = 0})
#define WS2812SPI_COLOR_GREEN ((WS2812SPI_Color){.red = 0, .green = 255, .blue = 0})
#define WS2812SPI_COLOR_BLUE ((WS2812SPI_Color){.red = 0, .green = 0, .blue = 255})
#define WS2812SPI_COLOR_WHITE ((WS2812SPI_Color){.red = 255, .green = 255, .blue = 255})
#define WS2812SPI_COLOR_BLACK ((WS2812SPI_Color){.red = 0, .green = 0, .blue = 0})

WS2812SPI_Handle* WS2812SPI_Entrance(SPI_HandleTypeDef *hspi, const WS2812SPI_Config *const config);

HAL_StatusTypeDef WS2812SPI_Exit(WS2812SPI_Handle *handle);

HAL_StatusTypeDef WS2812SPI_Write(WS2812SPI_Handle *handle, const WS2812SPI_Color *colors, uint16_t led_count);

#ifndef WS2812SPI_MAX_LEDS
#pragma warning "WS2812SPI_MAX_LEDS is not defined. Please define it in `main.h` to the maximum number of LEDs available on your MCU"
#define WS2812SPI_MAX_LEDS 32U
#endif

#ifndef WS2812SPI_MAX_SPI_BITS_PER_PIXEL
#pragma warning "WS2812SPI_MAX_SPI_BITS_PER_PIXEL is not defined. Please define it in `main.h` to use less than the maximum number of SPI bits per pixel available on your MCU"
#define WS2812SPI_MAX_SPI_BITS_PER_PIXEL 8U
#endif

#ifndef WS2812SPI_MAX_INSTANCES
#pragma warning "WS2812SPI_MAX_INSTANCES is not defined. Please define it in `main.h` to use less than the maximum number of SPI instances available on your MCU"
#if defined(SPI1) && defined(SPI2) && defined(SPI3) && defined(SPI4) && defined(SPI5) && defined(SPI6)
#define WS2812SPI_MAX_INSTANCES 6U
#elif defined(SPI1) && defined(SPI2) && defined(SPI3) && defined(SPI4) && defined(SPI5)
#define WS2812SPI_MAX_INSTANCES 5U
#elif defined(SPI1) && defined(SPI2) && defined(SPI3) && defined(SPI4)
#define WS2812SPI_MAX_INSTANCES 4U
#elif defined(SPI1) && defined(SPI2) && defined(SPI3)
#define WS2812SPI_MAX_INSTANCES 3U
#elif defined(SPI1) && defined(SPI2)
#define WS2812SPI_MAX_INSTANCES 2U
#elif defined(SPI1)
#define WS2812SPI_MAX_INSTANCES 1U
#else
#define WS2812SPI_MAX_INSTANCES 0U
#endif
#endif

#endif
