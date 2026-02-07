#include "main.h"

#ifndef GR_NEOPIXEL_H
#define GR_NEOPIXEL_H

#include <stdint.h>

typedef enum {
	COLOR_GREEN = (uint32_t)0xFF0000,
	COLOR_RED = (uint32_t)0x03FC00,
	COLOR_BLUE = (uint32_t)0x0000FF,
} Color;

typedef union {
	struct {
		Color TS_Active;
		Color RTD;
	};

	uint32_t rawData[2];
} NeoPixelData;

void Neopixel_update();

#endif // GR_NEOPIXEL_H

// make typedefs for all constants
// create initialization structs (one for GPIO, one for SPI)
// create a function to initialize SPI
// initialize spi in gr_neopixel.c

/*
	GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
	SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
	SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_4BIT;
	SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
	SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
	SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
	SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
	SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
	SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	SPI_InitStruct.CRCPoly = 7;
	LL_SPI_Init(SPI3, &SPI_InitStruct);
	LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_EnableNSSPulseMgt(SPI3);

*/
