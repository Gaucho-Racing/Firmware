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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCU2_TESTLED_GPIO_Port, MCU2_TESTLED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, U11_EN_Pin|U7_EN_Pin|U12_EN_Pin|U6_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : MCU2_TESTLED_Pin */
  GPIO_InitStruct.Pin = MCU2_TESTLED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MCU2_TESTLED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : U7_FLT_Pin U7_SPLYGD_Pin */
  GPIO_InitStruct.Pin = U7_FLT_Pin|U7_SPLYGD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : U11_FLT_Pin */
  GPIO_InitStruct.Pin = U11_FLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(U11_FLT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : U11_SPLYGD_Pin U7_IOUT_Pin U11_IOUT_Pin U6_SPLYGD_Pin */
  GPIO_InitStruct.Pin = U11_SPLYGD_Pin|U7_IOUT_Pin|U11_IOUT_Pin|U6_SPLYGD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : U11_EN_Pin U7_EN_Pin U12_EN_Pin U6_EN_Pin */
  GPIO_InitStruct.Pin = U11_EN_Pin|U7_EN_Pin|U12_EN_Pin|U6_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : U12_IOUT_Pin U6_IOUT_Pin U6_FLT_Pin U12_FLT_Pin */
  GPIO_InitStruct.Pin = U12_IOUT_Pin|U6_IOUT_Pin|U6_FLT_Pin|U12_FLT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : U12_SPLYGD_Pin _20V_IN_Sense_Pin CurrentSense_VOUT_Pin */
  GPIO_InitStruct.Pin = U12_SPLYGD_Pin|_20V_IN_Sense_Pin|CurrentSense_VOUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
