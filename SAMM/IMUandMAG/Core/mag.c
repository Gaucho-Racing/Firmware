#include "mag.h"
#include <stdio.h>

#include "stm32g474xx.h"
#include "stm32g4xx_hal_spi.h"

// init spi port before calling this function
uint8_t mag_init(mag *mag_dev, SPI_HandleTypeDef *spi_port, GPIO_TypeDef *port, uint16_t pin)
{
	uint8_t tx_word[4];
	uint8_t rx_word[4] = {0};
	uint8_t status = 0;
	mag_dev->spi_port = spi_port;
	mag_dev->port = port;
	mag_dev->pin = pin;
	tx_word[1] = (mag_CHIP_ID << 8);
	tx_word[1] |= 0x80;
	tx_word[0] = 0x69;
	/*
	Okay so for one of these transmits we need to follow the following operation:bmi323_dev
	1. to read the register we want to:
	  transmit first 8 bytes, then transmit a fake 8 bytes
	  after we want to read 16 bytes. This should complete a single read
	*/
	// first we read do the dummy read to switch to spi mode
	HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_RESET);
	status = HAL_SPI_TransmitReceive(bmi323_dev->spi_port, tx_word, rx_word, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_SET);
	// rx_word = 0;
	HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_RESET);
	status = HAL_SPI_TransmitReceive(bmi323_dev->spi_port, tx_word, rx_word, 2, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(bmi323_dev->port, bmi323_dev->pin, GPIO_PIN_SET);
	if (rx_word[3] == 0x43) {
		return HAL_OK;
	}
	return HAL_ERROR;
}
