/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#define BSE_SIGNAL_Pin GPIO_PIN_0
#define BSE_SIGNAL_GPIO_Port GPIOC
#define BSPD_SIGNAL_Pin GPIO_PIN_1
#define BSPD_SIGNAL_GPIO_Port GPIOC
#define APPS1_SIGNAL_Pin GPIO_PIN_2
#define APPS1_SIGNAL_GPIO_Port GPIOC
#define APPS2_SIGNAL_Pin GPIO_PIN_3
#define APPS2_SIGNAL_GPIO_Port GPIOC
#define BSPD_SENSE_Pin GPIO_PIN_5
#define BSPD_SENSE_GPIO_Port GPIOA
#define IMD_SENSE_Pin GPIO_PIN_6
#define IMD_SENSE_GPIO_Port GPIOA
#define AMS_SENSE_Pin GPIO_PIN_7
#define AMS_SENSE_GPIO_Port GPIOA
#define BRAKE_F_SIGNAL_Pin GPIO_PIN_0
#define BRAKE_F_SIGNAL_GPIO_Port GPIOB
#define BRAKE_R_SIGNAL_Pin GPIO_PIN_1
#define BRAKE_R_SIGNAL_GPIO_Port GPIOB
#define LED_TEST_Pin GPIO_PIN_11
#define LED_TEST_GPIO_Port GPIOB
#define AUX_SIGNAL_Pin GPIO_PIN_14
#define AUX_SIGNAL_GPIO_Port GPIOB
#define STEERING_ANGLE_Pin GPIO_PIN_15
#define STEERING_ANGLE_GPIO_Port GPIOB
#define BRAKE_LIGHT_Pin GPIO_PIN_4
#define BRAKE_LIGHT_GPIO_Port GPIOB
#define TSSI_G_CONTROL_Pin GPIO_PIN_5
#define TSSI_G_CONTROL_GPIO_Port GPIOB
#define TSSI_R_CONTROL_Pin GPIO_PIN_6
#define TSSI_R_CONTROL_GPIO_Port GPIOB
#define RTD_CONTROL_Pin GPIO_PIN_7
#define RTD_CONTROL_GPIO_Port GPIOB
#define AUX_CONTROL_Pin GPIO_PIN_8
#define AUX_CONTROL_GPIO_Port GPIOB
#define SOFTWARE_OK_CONTROL_Pin GPIO_PIN_9
#define SOFTWARE_OK_CONTROL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

// Undefined if one motor, controlled in `main.h`
#define ENABLE_THREE_MOTORS

// Uncomment to return to a 1 motor car
// #undef ENABLE_THREE_MOTORS

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
