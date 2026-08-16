/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_TEST_Pin|BRAKE_LIGHT_Pin|TSSI_G_CONTROL_Pin|TSSI_R_CONTROL_Pin
                          |RTD_CONTROL_Pin|AUX_CONTROL_Pin|SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RTD_BTN_LED_CONTROL_Pin|TS_ACTIVE_BTN_LED_CONTROL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_TEST_Pin BRAKE_LIGHT_Pin TSSI_G_CONTROL_Pin TSSI_R_CONTROL_Pin
                           RTD_CONTROL_Pin AUX_CONTROL_Pin SOFTWARE_OK_CONTROL_Pin */
  GPIO_InitStruct.Pin = LED_TEST_Pin|BRAKE_LIGHT_Pin|TSSI_G_CONTROL_Pin|TSSI_R_CONTROL_Pin
                          |RTD_CONTROL_Pin|AUX_CONTROL_Pin|SOFTWARE_OK_CONTROL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RTD_BTN_LED_CONTROL_Pin TS_ACTIVE_BTN_LED_CONTROL_Pin */
  GPIO_InitStruct.Pin = RTD_BTN_LED_CONTROL_Pin|TS_ACTIVE_BTN_LED_CONTROL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ESTOP_SENSE_Pin */
  GPIO_InitStruct.Pin = ESTOP_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ESTOP_SENSE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : INERTIA_SW_SENSE_Pin RTD_BTN_SENSE_Pin TS_ACTIVE_BTN_SENSE_Pin */
  GPIO_InitStruct.Pin = INERTIA_SW_SENSE_Pin|RTD_BTN_SENSE_Pin|TS_ACTIVE_BTN_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
