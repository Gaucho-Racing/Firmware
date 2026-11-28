#include <stdbool.h>
#include <string.h>

#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"
#include "ws2812spi.h"

static void WS2812SPI_Private_Pack(uint8_t *buffer, uint32_t *encoded_bit, uint8_t value, uint8_t width)
{
	if (width != 3U && width != 8U) {
		return;
	}

	value &= (uint8_t)((1U << width) - 1U);
	for (int8_t bit = (int8_t)(width - 1U); bit >= 0; --bit) {
		uint32_t byte_index = *encoded_bit / 8U;
		uint32_t bit_index = *encoded_bit % 8U;
		uint8_t output_bit = (value >> bit) & 1U;

		buffer[byte_index] |= (uint8_t)(output_bit << (7U - bit_index));
		++(*encoded_bit);
	}
}

static void WS2812SPI_Private_Encode_3b_Color(uint8_t *buffer, uint32_t *encoded_bit, uint8_t value)
{
	for (int8_t bit = 7; bit >= 0; --bit) {
		uint8_t neopixel_bit = (value >> bit) & 1U;
		uint8_t pattern = neopixel_bit ? 0x6U : 0x4U;
		WS2812SPI_Private_Pack(buffer, encoded_bit, pattern, 3U);
	}
}

static void WS2812SPI_Private_Encode_8b_Color(uint8_t *buffer, uint32_t *encoded_bit, uint8_t value)
{
	for (int8_t bit = 7; bit >= 0; --bit) {
		uint8_t neopixel_bit = (value >> bit) & 1U;
		buffer[*encoded_bit / 8U] = neopixel_bit ? 0xFFU : 0x00U;
		*encoded_bit += 8U;
	}
}

static uint8_t WS2812SPI_Private_Channel(const WS2812SPI_Private_Handle *handle, const WS2812SPI_Color *color, uint8_t channel_index)
{
	if (handle->config.color_order == WS2812SPI_ORDER_GRB) {
		static const uint8_t grb_order[3] = {1U, 0U, 2U};
		channel_index = grb_order[channel_index];
	}

	switch (channel_index) {
		case 0U:
			return color->red;
		case 1U:
			return color->green;
		default:
			return color->blue;
	}
}

HAL_StatusTypeDef WS2812SPI_Write(WS2812SPI_Handle *handle, const WS2812SPI_Color *colors, uint16_t led_count)
{
	WS2812SPI_Private_Handle *private_handle = (WS2812SPI_Private_Handle *)handle;

	if (private_handle == NULL || private_handle->started == false || colors == NULL) {
		LOGOMATIC_ERROR("WS2812SPI_Write: invalid parameters");
		return HAL_ERROR;
	}

	if (led_count == 0U || led_count > private_handle->config.led_count) {
		LOGOMATIC_ERROR("WS2812SPI_Write: invalid LED count");
		return HAL_ERROR;
	}

	if (private_handle->busy) {
		return HAL_BUSY;
	}

	memset(private_handle->encoded_buffer, 0, sizeof(private_handle->encoded_buffer));

	uint32_t encoded_bit = 0U;
	for (uint16_t led = 0U; led < led_count; ++led) {
		for (uint8_t channel = 0U; channel < 3U; ++channel) {
			uint8_t value = WS2812SPI_Private_Channel(private_handle, &colors[led], channel);
			if (private_handle->config.spi_bits_per_pixel == 3U) {
				WS2812SPI_Private_Encode_3b_Color(private_handle->encoded_buffer, &encoded_bit, value);
			} else {
				WS2812SPI_Private_Encode_8b_Color(private_handle->encoded_buffer, &encoded_bit, value);
			}
		}
	}

	private_handle->encoded_buffer_size = (uint16_t)((encoded_bit + 7U) / 8U);

	private_handle->busy = true;
	HAL_StatusTypeDef status;
	if (private_handle->spi->hdmatx != NULL) {
		status = HAL_SPI_Transmit_DMA(private_handle->spi, private_handle->encoded_buffer, private_handle->encoded_buffer_size);
	} else {
		status = HAL_SPI_Transmit_IT(private_handle->spi, private_handle->encoded_buffer, private_handle->encoded_buffer_size);
	}

	if (status != HAL_OK) {
		private_handle->busy = false;
		LOGOMATIC_ERROR("WS2812SPI_Write: SPI transmit failed");
	}

	return status;
}

__weak void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	for (uint32_t i = 0U; i < WS2812SPI_MAX_INSTANCES; ++i) {
		if (handles[i].spi == hspi) {
			handles[i].busy = false;
			return;
		}
	}
}
