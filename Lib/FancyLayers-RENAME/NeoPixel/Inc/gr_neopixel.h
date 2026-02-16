#include <stdint.h>

#include "main.h"

#ifndef GR_NEOPIXEL_H
#define GR_NEOPIXEL_H

/**
 * @brief Context containing all necessary information for controlling a Neopixel strip.
 * @note Acquired by calling Neopixel_Setup with a NeopixelConfig struct.
 * @note This struct is opaque to users of the library, its contents should not be accessed directly. All interactions with the Neopixel strip should be done through the provided functions in this library.
 */
typedef struct NeopixelContext NeopixelContext;

/**
 * @brief Configuration struct for Neopixel control. This should be initialized and passed to Neopixel_Setup before using any other functions in this library.
 * @todo Add fields for GPIO pin, SPI settings, etc.
 */
typedef struct {
	SPI_TypeDef *SPI_Instance;
	uint32_t SPI_FrequencyHz;	// FIXME This value should be calculated based off of passed in inputs used in MX_SPI1_Init
	uint32_t LatchTimeUs;
	uint32_t NumberOfNeopixels;
	// TODO - Add fields for GPIO pin, SPI settings, etc.
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

typedef struct NeopixelContext NeopixelContext;

/**
 * @brief Initializes the Neopixel library with the provided configuration. This must be called before any other functions in this library.
 * @note This function will allocate memory for the NeopixelContext struct. There is no corresponding de-initialization function, this is not an issue in practice since this is only expected to be called once at the beginning of the program.
 * @param neopixelConfiguration A pointer to a NeopixelConfig struct containing the desired configuration for the Neopixel library.
 * @return A pointer to a NeopixelContext struct that can be used for subsequent operations on the Neopixel strip.
 */
NeopixelContext* Neopixel_Setup(NeopixelConfig *neopixelConfiguration);

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
