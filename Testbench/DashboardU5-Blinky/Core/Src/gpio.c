/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
 */
void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PE0 PE4 PE1 PE3
				 PE6 PE2 PE8 PE13
				 PE7 PE15 PE12 PE9
				 PE10 PE11 PE14 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_4 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_15 | GPIO_PIN_12 | GPIO_PIN_9 |
			      GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : PB7 PB6 PB5 PB9
				 PB8 PB4 PB3 PB1
				 PB11 PB12 PB13 PB0
				 PB10 PB14 PB2 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_1 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_0 |
			      GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_2 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PG15 PG12 PG9 PG13
				 PG10 PG11 PG14 PG7
				 PG6 PG3 PG8 PG5
				 PG4 PG2 PG1 PG0 */
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_12 | GPIO_PIN_9 | GPIO_PIN_13 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_5 |
			      GPIO_PIN_4 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : PD4 PD3 PD5 PD2
				 PD6 PD1 PD7 PD0
				 PD15 PD11 PD12 PD8
				 PD13 PD14 PD10 PD9 */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_0 | GPIO_PIN_15 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_8 |
			      GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_10 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : PC10 PC8 PC14 PC11
				 PC7 PC15 PC12 PC9
				 PC6 PC13 PC1 PC3
				 PC2 PC0 PC5 PC4 */
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_14 | GPIO_PIN_11 | GPIO_PIN_7 | GPIO_PIN_15 | GPIO_PIN_12 | GPIO_PIN_9 | GPIO_PIN_6 | GPIO_PIN_13 | GPIO_PIN_1 | GPIO_PIN_3 |
			      GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_5 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA15 PA14 PA8 PA12
				 PA11 PA13 PA10 PA9
				 PA2 PA4 PA0 PA1
				 PA3 PA6 PA5 PA7 */
	GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_8 | GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_0 | GPIO_PIN_1 |
			      GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PH3 PH7 PH5 PH6
				 PH4 PH8 PH2 PH14
				 PH15 PH10 PH11 PH13
				 PH9 PH12 PH0 PH1 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_7 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_8 | GPIO_PIN_2 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_13 |
			      GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/*Configure GPIO pins : PI5 PI7 PI6 PI15
				 PI14 PI13 PI12 PI11
				 PI10 PI9 PI8 PI4
				 PI1 PI2 PI3 PI0 */
	GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13 | GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_8 | GPIO_PIN_4 |
			      GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

	/*Configure GPIO pins : PJ3 PJ2 PJ7 PJ6
				 PJ4 PJ0 PJ10 PJ8
				 PJ5 PJ11 PJ1 PJ9 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_0 | GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_1 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	/*Configure GPIO pin : USER_LED_Pin */
	GPIO_InitStruct.Pin = USER_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PF0 PF2 PF1 PF4
				 PF3 PF10 PF5 PF7
				 PF9 PF6 PF8 PF13
				 PF12 PF11 PF15 PF14 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_10 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_13 |
			      GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_15 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
