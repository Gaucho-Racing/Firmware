/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SPI_HandleTypeDef hspi3;

/* spi3 init function */
void MX_SPI3_Init(void)
{

	/* USER CODE BEGIN spi3_Init 0 */

	/* USER CODE END spi3_Init 0 */

	/* USER CODE BEGIN spi3_Init 1 */

	/* USER CODE END spi3_Init 1 */
	hspi3.Instance = SPI3;
	hspi3.Init.Mode = SPI_MODE_MASTER;
	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
	hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi3.Init.NSS = SPI_NSS_SOFT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi3.Init.CRCPolynomial = 7;
	hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi3) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN spi3_Init 2 */

	/* USER CODE END spi3_Init 2 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (spiHandle->Instance == spi3) {
		/* USER CODE BEGIN spi3_MspInit 0 */

		/* USER CODE END spi3_MspInit 0 */
		/* spi3 clock enable */
		__HAL_RCC_spi3_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		/**spi3 GPIO Configuration
		PA5     ------> spi3_SCK
		PA6     ------> spi3_MISO
		PA7     ------> spi3_MOSI
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF4_spi3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF5_spi3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_spi3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USER CODE BEGIN spi3_MspInit 1 */

		/* USER CODE END spi3_MspInit 1 */
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle)
{

	if (spiHandle->Instance == spi3) {
		/* USER CODE BEGIN spi3_MspDeInit 0 */

		/* USER CODE END spi3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_spi3_CLK_DISABLE();

		/**spi3 GPIO Configuration
		PC10     ------> spi3_SCK
		PC11     ------> spi3_MISO
		PC12     ------> spi3_MOSI
		*/
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);

		/* USER CODE BEGIN spi3_MspDeInit 1 */

		/* USER CODE END spi3_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
