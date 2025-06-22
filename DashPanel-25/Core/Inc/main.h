/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32u5xx_hal.h"

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
#define NO_RTD_Pin GPIO_PIN_6
#define NO_RTD_GPIO_Port GPIOB
#define LED_IMD_Pin GPIO_PIN_9
#define LED_IMD_GPIO_Port GPIOG
#define LED_AMS_Pin GPIO_PIN_10
#define LED_AMS_GPIO_Port GPIOG
#define LCD_DISP_RESET_Pin GPIO_PIN_7
#define LCD_DISP_RESET_GPIO_Port GPIOH
#define CTP_RST_Pin GPIO_PIN_3
#define CTP_RST_GPIO_Port GPIOE
#define CTP_INT_Pin GPIO_PIN_6
#define CTP_INT_GPIO_Port GPIOE
#define CTP_INT_EXTI_IRQn EXTI6_IRQn
#define USB_PWR_EN_Pin GPIO_PIN_8
#define USB_PWR_EN_GPIO_Port GPIOG
#define USB_OVERCURRENT_Pin GPIO_PIN_0
#define USB_OVERCURRENT_GPIO_Port GPIOJ
#define DIN_TS_ACTIVE_Pin GPIO_PIN_4
#define DIN_TS_ACTIVE_GPIO_Port GPIOG
#define R_RST_Pin GPIO_PIN_4
#define R_RST_GPIO_Port GPIOF
#define NO_TS_ACTIVE_Pin GPIO_PIN_11
#define NO_TS_ACTIVE_GPIO_Port GPIOH
#define NC_RTD_Pin GPIO_PIN_4
#define NC_RTD_GPIO_Port GPIOA
#define R_CS_Pin GPIO_PIN_1
#define R_CS_GPIO_Port GPIOG
#define LED_BSPD_Pin GPIO_PIN_3
#define LED_BSPD_GPIO_Port GPIOA
#define RTD_Pin GPIO_PIN_6
#define RTD_GPIO_Port GPIOA
#define TS_ACTIVE_Pin GPIO_PIN_7
#define TS_ACTIVE_GPIO_Port GPIOA
#define R_INT_Pin GPIO_PIN_4
#define R_INT_GPIO_Port GPIOC
#define NC_TS_ACTIVE_Pin GPIO_PIN_15
#define NC_TS_ACTIVE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
// Undefined if one motor, controlled in `main.h`
#define ENABLE_THREE_MOTORS

// Uncomment to return to a 1 motor car
// #undef ENABLE_THREE_MOTORS

#define GR_COLOR_PINK 0xEF0DA1
#define GR_COLOR_PURPLE 0x7920FF
#define GR_COLOR_NAVY 0x195297
#define GR_COLOR_GRAY 0x9AA3B0
#define GR_COLOR_WHITE 0xFFFFFF
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
