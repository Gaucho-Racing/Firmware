#include "gr_neopixel.h"

#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>

#include "Logomatic.h"
#include "main.h"

struct NeopixelContext {
	// Configuration provided at setup
	NeopixelConfig config;
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

/**
 * @brief Internal function to delay execution between transmissions, using BlockWhileBusy.
 * @note 1 ms delay is longer than required between data refresh, but more convenient than implementing minimum (50 us)
 * @param context A pointer to the NeopixelContext containing the SPI instance to check.
 * @return none
 */
void Neopixel_LatchStrip(NeopixelContext *context)
{
	Neopixel_BlockWhileBusy(context);
	LL_mDelay(1);
	Neopixel_BlockWhileBusy(context);
}

/**
 * @brief Initialize GPIO and SPI for Neopixel based on neopixelConfiguration
 * @param neopixelConfiguration A pointer to the NeopixelConfig containing all customizable parameters, must be initialized by user
 * @return A pointer to NeopixelContext, which contains the set up from neopixelConfiguration
 */
NeopixelContext *Neopixel_Setup(NeopixelConfig *neopixelConfiguration)
{
	if (neopixelConfiguration == NULL) {
		LOGOMATIC("Neopixel configuration is NULL!\n");
		return NULL;
	}

	// Enable clocks for GPIO depending on port used
	GPIO_TypeDef *gpio_port = 0;
	switch (neopixelConfiguration->GPIO_Port) {
		case Neopixel_GPIOA:
			gpio_port = GPIOA;
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
			break;
		case Neopixel_GPIOB:
			gpio_port = GPIOB;
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
			break;
		case Neopixel_GPIOC:
			gpio_port = GPIOC;
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
			break;
		case Neopixel_GPIOD:
			gpio_port = GPIOD;
			LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
			break;
		default:
			return NULL;
	}

	// Set up and initialize GPIO
	LL_GPIO_InitTypeDef copi_pin = {
	    .Pin = neopixelConfiguration->MOSI_Pin,
	    .Mode = LL_GPIO_MODE_ALTERNATE,
	    .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
	    .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
	    .Pull = LL_GPIO_PULL_NO,
	    .Alternate = neopixelConfiguration->Neopixel_Count,
	};

	LL_GPIO_Init(gpio_port, &copi_pin);

	// Set up and initialize SPI
	LL_SPI_InitTypeDef spi = {
	    .TransferDirection = LL_SPI_HALF_DUPLEX_TX,
	    .Mode = LL_SPI_MODE_MASTER,
	    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
	    .ClockPolarity = LL_SPI_POLARITY_LOW,
	    .ClockPhase = LL_SPI_PHASE_1EDGE,
	    .NSS = LL_SPI_NSS_SOFT,
	    .BaudRate = neopixelConfiguration->SPI_BaudRatePrescaler,
	    .BitOrder = LL_SPI_MSB_FIRST,
	    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
	    .CRCPoly = 7,
	};

	// Enable clocks for SPI depending on SPI instance used
	if (neopixelConfiguration->SPI_Instance == SPI1) {
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	} else if (neopixelConfiguration->SPI_Instance == SPI2) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
	} else if (neopixelConfiguration->SPI_Instance == SPI3) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	}

	LL_SPI_Init(neopixelConfiguration->SPI_Instance, &spi);
	LL_SPI_SetStandard(neopixelConfiguration->SPI_Instance, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_EnableNSSPulseMgt(neopixelConfiguration->SPI_Instance);
	LL_SPI_Enable(neopixelConfiguration->SPI_Instance);

	NeopixelContext *context = malloc(sizeof(NeopixelContext));

	if (context == NULL) {
		LOGOMATIC("Failed to allocate memory for Neopixel context!\n");
		return NULL;
	}

	context->config = *neopixelConfiguration;

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
	if (context == NULL) {
		LOGOMATIC("Context is NULL!\n");
		return;
	}
	if (colors == NULL) {
		LOGOMATIC("Colors array is NULL!\n");
		return;
	}
	if (sizeofColors == 0) {
		LOGOMATIC("Size of colors array is 0!\n");
		return;
	}

	if (context->config.Neopixel_Count * sizeof(Neopixel_Color) != sizeofColors) {
		LOGOMATIC("Number of colors provided does not match number of Neopixels configured!\n");
		LOGOMATIC("Expected %lu colors, got %lu colors\n", context->config.Neopixel_Count, sizeofColors / sizeof(Neopixel_Color));
		assert_param(context->config.NumberOfNeopixels * sizeof(Neopixel_Color) == sizeofColors);
		return;
	}

	uint8_t neopixelTransmission[context->config.Neopixel_Count * 24];
	for (uint32_t i = 0; i < context->config.Neopixel_Count; i++) {
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
