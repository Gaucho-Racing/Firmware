/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MCU3_TESTLED_Pin GPIO_PIN_14
#define MCU3_TESTLED_GPIO_Port GPIOC
#define U14_FLT_Pin GPIO_PIN_0
#define U14_FLT_GPIO_Port GPIOF
#define U14_SPLYGD_Pin GPIO_PIN_1
#define U14_SPLYGD_GPIO_Port GPIOF
#define U15_FLT_Pin GPIO_PIN_10
#define U15_FLT_GPIO_Port GPIOG
#define U15_SPLYGD_Pin GPIO_PIN_0
#define U15_SPLYGD_GPIO_Port GPIOA
#define U15_EN_Pin GPIO_PIN_1
#define U15_EN_GPIO_Port GPIOA
#define U14_EN_Pin GPIO_PIN_3
#define U14_EN_GPIO_Port GPIOA
#define U14_IOUT_Pin GPIO_PIN_4
#define U14_IOUT_GPIO_Port GPIOA
#define U15_IOUT_Pin GPIO_PIN_5
#define U15_IOUT_GPIO_Port GPIOA
#define U16_IOUT_Pin GPIO_PIN_12
#define U16_IOUT_GPIO_Port GPIOB
#define U13_IOUT_Pin GPIO_PIN_13
#define U13_IOUT_GPIO_Port GPIOB
#define U13_FLT_Pin GPIO_PIN_14
#define U13_FLT_GPIO_Port GPIOB
#define U16_FLT_Pin GPIO_PIN_15
#define U16_FLT_GPIO_Port GPIOB
#define U16_SPLYGD_Pin GPIO_PIN_6
#define U16_SPLYGD_GPIO_Port GPIOC
#define U16_EN_Pin GPIO_PIN_8
#define U16_EN_GPIO_Port GPIOA
#define U13_SPLYGD_Pin GPIO_PIN_9
#define U13_SPLYGD_GPIO_Port GPIOA
#define U13_EN_Pin GPIO_PIN_10
#define U13_EN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
