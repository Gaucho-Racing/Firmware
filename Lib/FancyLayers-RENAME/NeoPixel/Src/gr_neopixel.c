#include "gr_neopixel.h"

#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "Logomatic.h"
#include "main.h"

#define BITS_PER_BYTE (8U)
#define MICROSECONDS_PER_SECOND (1000000UL)

struct NeopixelContext {
	// Configuration provided at setup
	NeopixelConfig config;
	// Number of zero bytes to send after color data to ensure latching, cached calculation from config parameters
	uint32_t zeroTailBytes;
};

/**
 * @brief Internal function to block execution until the SPI peripheral is no longer busy. This is used to ensure that we don't start a new transmission before the previous one has completed.
 * @param context A pointer to the NeopixelContext containing the SPI instance to check.
 * @return None
 * @warning This function will block indefinitely if the SPI peripheral gets stuck in a busy state. This can happen if the SPI bus is not setup correctly.
 */
static void Neopixel_BlockWhileBusy(NeopixelContext *context)
{
	while (LL_SPI_IsActiveFlag_BSY(context->config.SPI_Instance)) {}
}

void Neopixel_LatchStrip(NeopixelContext *context)
{
	Neopixel_BlockWhileBusy(context);
	for (uint32_t i = 0; i < context->zeroTailBytes; i++) {
		while (!LL_SPI_IsActiveFlag_TXE(context->config.SPI_Instance)) {}
		LL_SPI_TransmitData8(context->config.SPI_Instance, 0x00);
	}
	Neopixel_BlockWhileBusy(context);
}

NeopixelContext *Neopixel_Setup(NeopixelConfig *neopixelConfiguration)
{
	// TODO Abstraction
	// - Add internal enums instead of using provided preprocessor values
	// - Initialize GPIO for SPI from config
	// - Initialize SPI peripheral with settings from config
	//
	// Essentially, main.c should be able to call this function with a config struct and not have to worry about the details of SPI or GPIO initialization.
	// This will make it easier to reuse this code across different projects and microcontrollers in the future.
	//
	// TLDR
	// Remove the call to MX_SPI1_Init from main.c, setup the SPI peripheral with the settings being determined by the NeopixelConfig struct passed in.
	// NeopixelConfig should have the minimum necessary information to fully initialize the SPI peripheral for Neopixel control.

	// SETUP GPIO

	// SETUP SPI

	// ENABLE CLOCKS

	// ENABLE SPI PERIPHERAL

	if (neopixelConfiguration == NULL) {
		LOGOMATIC("Neopixel configuration is NULL!\n");
		return NULL;
	}

	NeopixelContext *context = malloc(sizeof(NeopixelContext));

	if (context == NULL) {
		LOGOMATIC("Failed to allocate memory for Neopixel context!\n");
		return NULL;
	}

	context->config = *neopixelConfiguration;

	// Calculate timings

	const uint64_t zerotail_divisor = BITS_PER_BYTE * MICROSECONDS_PER_SECOND;
	const uint64_t zerotail_numerator = (uint64_t)context->config.SPI_FrequencyHz * (uint64_t)context->config.LatchTimeUs + zerotail_divisor - 1ULL;
	context->zeroTailBytes = (uint32_t)(zerotail_numerator / zerotail_divisor);

	Neopixel_LatchStrip(context);

	return context;
}

/**
 * @brief Internal function to encode a 24-bit GRB color into the format required for Neopixel transmission. The encoded color is written to the provided buffer, which must be at least 24 bytes long.
 * @param buffer A pointer to a buffer where the encoded color will be written. This buffer must be at least 24 bytes long.
 * @param color A 24-bit GRB color to encode. The format of the color should be 0x00GGRRBB, where GG is the green component, RR is the red component, and BB is the blue component.
 */
static void Neopixel_EncodeColor(uint8_t *buffer, uint32_t color)
{
	for (int i = 23; i >= 0; i--) {
		buffer[23 - i] = 0x4 + (((color >> i) & 0x1U) << 1);
	}
}

void Neopixel_WriteAll(NeopixelContext *context, const Neopixel_Color *colors, uint32_t sizeofColors)
{
	// TODO Add null checks for all parameters

	if (context->config.NumberOfNeopixels * sizeof(Neopixel_Color) != sizeofColors) {
		LOGOMATIC("Number of colors provided does not match number of Neopixels configured!\n");
		LOGOMATIC("Expected %lu colors, got %lu colors\n", context->config.NumberOfNeopixels, sizeofColors / sizeof(Neopixel_Color));
		assert_param(context->config.NumberOfNeopixels * sizeof(Neopixel_Color) == sizeofColors);
		return;
	}

	uint8_t neopixelTransmission[context->config.NumberOfNeopixels * 24];
	for (uint32_t i = 0; i < context->config.NumberOfNeopixels; i++) {
		Neopixel_EncodeColor(&neopixelTransmission[i * 24], colors[i]);
	}

	Neopixel_BlockWhileBusy(context);

	for (uint32_t i = 0; i < sizeof(neopixelTransmission); i++) {
		while (!LL_SPI_IsActiveFlag_TXE(context->config.SPI_Instance)) {}
		LL_SPI_TransmitData8(context->config.SPI_Instance, neopixelTransmission[i]);
	}

	Neopixel_LatchStrip(context);

	Neopixel_BlockWhileBusy(context);
}
