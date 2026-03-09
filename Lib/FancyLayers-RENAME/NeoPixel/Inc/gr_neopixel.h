#include <stdint.h>

#include "main.h"

#ifndef GR_NEOPIXEL_H
#define GR_NEOPIXEL_H

/**
 * @brief Port used by Neopixel GPIO pins.
 * @note Same port must be used by all GPIO pins associated with one NeopixelConfig struct.
 */
typedef enum {
	Neopixel_GPIOA,
	Neopixel_GPIOB,
	Neopixel_GPIOC,
	Neopixel_GPIOD
} Neopixel_GPIO_Port;

/**
 * @brief Alternate function for a specific GPIO pin and port.
 * @note AF5 used for SPI1/SPI2, AF6 used for SPI3.
 */
typedef enum {
	Neopixel_AF0 = LL_GPIO_AF_0,
	Neopixel_AF1 = LL_GPIO_AF_1,
	Neopixel_AF2 = LL_GPIO_AF_2,
	Neopixel_AF3 = LL_GPIO_AF_3,
	Neopixel_AF4 = LL_GPIO_AF_4,
	Neopixel_AF5 = LL_GPIO_AF_5,
	Neopixel_AF6 = LL_GPIO_AF_6,
	Neopixel_AF7 = LL_GPIO_AF_7,
	Neopixel_AF8 = LL_GPIO_AF_8,
	Neopixel_AF9 = LL_GPIO_AF_9,
	Neopixel_AF10 = LL_GPIO_AF_10,
	Neopixel_AF11 = LL_GPIO_AF_11,
	Neopixel_AF12 = LL_GPIO_AF_12,
	Neopixel_AF13 = LL_GPIO_AF_13,
	Neopixel_AF14 = LL_GPIO_AF_14,
	Neopixel_AF15 = LL_GPIO_AF_15
} Neopixel_GPIO_AlternateFunction;

/**
 * @brief Baud rate prescaler for SPI (division factor for system clock).
 */
typedef enum {
	Neopixel_SPI_PS2 = LL_SPI_BAUDRATEPRESCALER_DIV2,
	Neopixel_SPI_PS4 = LL_SPI_BAUDRATEPRESCALER_DIV4,
	Neopixel_SPI_PS8 = LL_SPI_BAUDRATEPRESCALER_DIV8,
	Neopixel_SPI_PS16 = LL_SPI_BAUDRATEPRESCALER_DIV16,
	Neopixel_SPI_PS32 = LL_SPI_BAUDRATEPRESCALER_DIV32,
	Neopixel_SPI_PS64 = LL_SPI_BAUDRATEPRESCALER_DIV64,
	Neopixel_SPI_PS128 = LL_SPI_BAUDRATEPRESCALER_DIV128,
	Neopixel_SPI_PS256 = LL_SPI_BAUDRATEPRESCALER_DIV256
} Neopixel_SPI_BaudRatePrescaler;

/**
 * @brief Context containing all necessary information for controlling a Neopixel strip.
 * @note Acquired by calling Neopixel_Setup with a NeopixelConfig struct.
 * @note This struct is opaque to users of the library, its contents should not be accessed directly. All interactions with the Neopixel strip should be done through the provided functions in this
 * library.
 */
typedef struct NeopixelContext NeopixelContext;

/**
 * @brief Configuration struct for Neopixel control. This should be initialized and passed to Neopixel_Setup before using any other functions in this library.
 */
typedef struct {
	SPI_TypeDef *SPI_Instance;
	uint32_t Neopixel_Count;
	uint32_t MOSI_Pin;
	Neopixel_GPIO_AlternateFunction GPIO_AlternateFunction;
	Neopixel_GPIO_Port GPIO_Port;
	uint32_t SPI_BaudRatePrescaler;
} NeopixelConfig;

/**
 * @brief Encodes a 24-bit GRB color into the format required for Neopixel transmission.
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

#endif
