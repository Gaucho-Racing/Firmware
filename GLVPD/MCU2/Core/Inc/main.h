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
#define MCU2_TESTLED_Pin GPIO_PIN_14
#define MCU2_TESTLED_GPIO_Port GPIOC
#define U7_FLT_Pin GPIO_PIN_0
#define U7_FLT_GPIO_Port GPIOF
#define U7_SPLYGD_Pin GPIO_PIN_1
#define U7_SPLYGD_GPIO_Port GPIOF
#define U11_FLT_Pin GPIO_PIN_0
#define U11_FLT_GPIO_Port GPIOA
#define U11_SPLYGD_Pin GPIO_PIN_1
#define U11_SPLYGD_GPIO_Port GPIOA
#define U11_EN_Pin GPIO_PIN_2
#define U11_EN_GPIO_Port GPIOA
#define U7_EN_Pin GPIO_PIN_3
#define U7_EN_GPIO_Port GPIOA
#define U7_IOUT_Pin GPIO_PIN_4
#define U7_IOUT_GPIO_Port GPIOA
#define U11_IOUT_Pin GPIO_PIN_5
#define U11_IOUT_GPIO_Port GPIOA
#define CurrentSense_VOUT_Pin GPIO_PIN_0
#define CurrentSense_VOUT_GPIO_Port GPIOB
#define _20V_IN_Sense_Pin GPIO_PIN_1
#define _20V_IN_Sense_GPIO_Port GPIOB
#define U12_IOUT_Pin GPIO_PIN_11
#define U12_IOUT_GPIO_Port GPIOB
#define U6_IOUT_Pin GPIO_PIN_12
#define U6_IOUT_GPIO_Port GPIOB
#define U6_FLT_Pin GPIO_PIN_14
#define U6_FLT_GPIO_Port GPIOB
#define U12_FLT_Pin GPIO_PIN_15
#define U12_FLT_GPIO_Port GPIOB
#define U12_SPLYGD_Pin GPIO_PIN_6
#define U12_SPLYGD_GPIO_Port GPIOC
#define U12_EN_Pin GPIO_PIN_8
#define U12_EN_GPIO_Port GPIOA
#define U6_SPLYGD_Pin GPIO_PIN_9
#define U6_SPLYGD_GPIO_Port GPIOA
#define U6_EN_Pin GPIO_PIN_10
#define U6_EN_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
