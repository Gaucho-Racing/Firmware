#include <stdint.h>

#include "main.h"

#ifndef GR_NEOPIXEL_H
#define GR_NEOPIXEL_H

// @brief Pin to configure GPIO on for NeoPixel
typedef enum {
	GPIO_PIN_0 = LL_GPIO_PIN_0,
	GPIO_PIN_1 = LL_GPIO_PIN_1,
	GPIO_PIN_2 = LL_GPIO_PIN_2,
	GPIO_PIN_3 = LL_GPIO_PIN_3,
	GPIO_PIN_4 = LL_GPIO_PIN_4,
	GPIO_PIN_5 = LL_GPIO_PIN_5,
	GPIO_PIN_6 = LL_GPIO_PIN_6,
	GPIO_PIN_7 = LL_GPIO_PIN_7,
	GPIO_PIN_8 = LL_GPIO_PIN_8,
	GPIO_PIN_9 = LL_GPIO_PIN_9,
	GPIO_PIN_10 = LL_GPIO_PIN_10,
	GPIO_PIN_11 = LL_GPIO_PIN_11,
	GPIO_PIN_12 = LL_GPIO_PIN_12,
	GPIO_PIN_13 = LL_GPIO_PIN_13,
	GPIO_PIN_14 = LL_GPIO_PIN_14,
	GPIO_PIN_15 = LL_GPIO_PIN_15
} GPIO_Pins;

/// @brief Alternate function for a specific pin and specific port
// Technically can be set for AF 0-15 for low and high registers separately
typedef enum {
	GPIO_AF_0 = LL_GPIO_AF_0,
	GPIO_AF_1 = LL_GPIO_AF_1,
	GPIO_AF_2 = LL_GPIO_AF_2,
	GPIO_AF_3 = LL_GPIO_AF_3,
	GPIO_AF_4 = LL_GPIO_AF_4,
	GPIO_AF_5 = LL_GPIO_AF_5,
	GPIO_AF_6 = LL_GPIO_AF_6,
	GPIO_AF_7 = LL_GPIO_AF_7,
	GPIO_AF_8 = LL_GPIO_AF_8,
	GPIO_AF_9 = LL_GPIO_AF_9,
	GPIO_AF_10 = LL_GPIO_AF_10,
	GPIO_AF_11 = LL_GPIO_AF_11,
	GPIO_AF_12 = LL_GPIO_AF_12,
	GPIO_AF_13 = LL_GPIO_AF_13,
	GPIO_AF_14 = LL_GPIO_AF_14,
	GPIO_AF_15 = LL_GPIO_AF_15
} GPIO_Alternate_Function;



/**
 * @brief Context containing all necessary information for controlling a Neopixel strip.
 * @note Acquired by calling Neopixel_Setup with a NeopixelConfig struct.
 * @note This struct is opaque to users of the library, its contents should not be accessed directly. All interactions with the Neopixel strip should be done through the provided functions in this
 * library.
 */
typedef struct NeopixelContext NeopixelContext;

/**
 * @brief Configuration struct for Neopixel control. This should be initialized and passed to Neopixel_Setup before using any other functions in this library.
 * @todo Add fields for GPIO pin, SPI settings, etc.
 */
typedef struct {
	SPI_TypeDef *SPI_Instance;
	uint32_t SPI_FrequencyHz; // FIXME This value should be calculated based off of passed in inputs used in MX_SPI1_Init
	uint32_t LatchTimeUs;
	uint32_t NumberOfNeopixels;
	// TODO - Add fields for GPIO pin, SPI settings, etc.
	//
} NeopixelConfig;

/**
 * @brief Encodes a 24-bit GRB color into the format required for Neopixel transmission.
 * @todo Add more experimentally found nice looking predetermined color options.
 */
typedef enum {
	COLOR_OFF = (uint32_t)0x000000,
	COLOR_RED = (uint32_t)0x03FC00,
	COLOR_ORANGE = (uint32_t)0x80FF00,
	COLOR_YELLOW = (uint32_t)0xFFFF00,
	COLOR_GREEN = (uint32_t)0xFF0000,
	COLOR_BLUE = (uint32_t)0x0000FF,
	COLOR_PURPLE = (uint32_t)0x00FF7F,
	COLOR_WHITE = (uint32_t)0xFFFFFF,
} Neopixel_Color;

/**
 * @brief Initializes the Neopixel library with the provided configuration. This must be called before any other functions in this library.
 * @note This function will allocate memory for the NeopixelContext struct. There is no corresponding de-initialization function, this is not an issue in practice since this is only expected to be
 * called once at the beginning of the program.
 * @param neopixelConfiguration A pointer to a NeopixelConfig struct containing the desired configuration for the Neopixel library.
 * @return A pointer to a NeopixelContext struct that can be used for subsequent operations on the Neopixel strip.
 */
NeopixelContext *Neopixel_Setup(NeopixelConfig *neopixelConfiguration);

/**
 * @brief Writes an array of colors to the Neopixel strip. The number of colors must match the number of Neopixels configured in Neopixel_Setup.
 * @note This function will block until the transmission is complete.
 * @param colors An array of Neopixel_Color values to write to the strip.
 * @param sizeofColors The total size of the colors array in bytes. This should correspond to the size of the NumberOfNeopixels field in the NeopixelConfig struct used to initialize the library.
 * @return None
 */
void Neopixel_WriteAll(NeopixelContext *context, const Neopixel_Color *colors, uint32_t sizeofColors);

// make typedefs for all constants
// create initialization structs (one for GPIO, one for SPI)
// create a function to initialize SPI
// initialize spi in gr_neopixel.c


#endif

/* for quick access
void MX_SPI1_Init(void)
{
	LL_GPIO_InitTypeDef copi_pin = {
	    .Pin = NEOPIXEL_DIN_Pin, //adjustable?
	    .Mode = LL_GPIO_MODE_ALTERNATE,
	    .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
	    .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
	    .Pull = LL_GPIO_PULL_NO,
	    .Alternate = LL_GPIO_AF_5, // adjustable?
	};
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); //adjustable?
	LL_GPIO_Init(NEOPIXEL_DIN_GPIO_Port, &copi_pin);

	LL_SPI_InitTypeDef sp1 = {
	    .TransferDirection = LL_SPI_HALF_DUPLEX_TX,
	    .Mode = LL_SPI_MODE_MASTER,
	    .DataWidth = LL_SPI_DATAWIDTH_8BIT,
	    .ClockPolarity = LL_SPI_POLARITY_LOW,
	    .ClockPhase = LL_SPI_PHASE_1EDGE,
	    .NSS = LL_SPI_NSS_SOFT,
	    .BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64,
	    .BitOrder = LL_SPI_MSB_FIRST,
	    .CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE,
	    .CRCPoly = 7,
	};
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1); //adjustable?
	LL_SPI_Init(SPI1, &sp1);
	LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_EnableNSSPulseMgt(SPI1);
	LL_SPI_Enable(SPI1);
}
*/
