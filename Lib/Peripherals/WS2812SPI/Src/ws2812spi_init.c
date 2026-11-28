#include <stdint.h>
#include <string.h>

#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"
#include "spi.h"
#include "ws2812spi.h"

WS2812SPI_Handle *WS2812SPI_Entrance(SPI_HandleTypeDef *hspi, const WS2812SPI_Config *const config)
{
	if (hspi == NULL) {
		LOGOMATIC_ERROR("WS2812SPI: SPI handle is NULL");
		return NULL;
	}

	if (config == NULL) {
		LOGOMATIC_ERROR("WS2812SPI: Config is NULL");
		return NULL;
	}

	if (config->led_count == 0U || config->led_count > WS2812SPI_MAX_LEDS) {
		LOGOMATIC_ERROR("WS2812SPI_Entrance: led_count is out of range");
		return NULL;
	}

	if (config->spi_bits_per_pixel != 3U &&
	    config->spi_bits_per_pixel != 8U) {
		LOGOMATIC_ERROR("WS2812SPI_Entrance: SPI bits per pixel must be 3 or 8");
		return NULL;
	}

	if (config->color_order != WS2812SPI_ORDER_RGB && config->color_order != WS2812SPI_ORDER_GRB) {
		LOGOMATIC_ERROR("WS2812SPI_Entrance: unsupported color order");
		return NULL;
	}

	for (uint32_t i = 0U; i < WS2812SPI_MAX_INSTANCES; ++i) {
		if (handles[i].spi == hspi) {
			LOGOMATIC_ERROR("WS2812SPI_Entrance: SPI handle is already initialized");
			return NULL;
		}
	}

	for (uint32_t i = 0U; i < WS2812SPI_MAX_INSTANCES; ++i) {
		if (!handles[i].started) {
			memset(&handles[i], 0, sizeof(handles[i]));
			handles[i].spi = hspi;
			handles[i].config = *config;
			handles[i].started = true;
			return &handles[i];
		}
	}

	LOGOMATIC_ERROR("WS2812SPI_Entrance: no free handle slots");
	return NULL;
}

HAL_StatusTypeDef WS2812SPI_Exit(WS2812SPI_Handle *handle)
{
	WS2812SPI_Private_Handle *private_handle = (WS2812SPI_Private_Handle *)handle;

	if (private_handle == NULL || private_handle->started == false) {
		LOGOMATIC_ERROR("WS2812SPI_Exit: invalid handle");
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_OK;
	if (private_handle->busy) {
		status = HAL_SPI_Abort(private_handle->spi);
	}

	if (status == HAL_OK) {
		memset(private_handle, 0, sizeof(*private_handle));
	}

	return status;
}
