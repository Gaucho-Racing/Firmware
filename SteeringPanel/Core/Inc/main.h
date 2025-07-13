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
#define TME_P3_Pin GPIO_PIN_9
#define TME_P3_GPIO_Port GPIOG
#define CME_P1_Pin GPIO_PIN_15
#define CME_P1_GPIO_Port GPIOA
#define RME_P4_Pin GPIO_PIN_8
#define RME_P4_GPIO_Port GPIOA
#define CME_P4_Pin GPIO_PIN_5
#define CME_P4_GPIO_Port GPIOB
#define TME_P2_Pin GPIO_PIN_10
#define TME_P2_GPIO_Port GPIOG
#define LCD_DISP_RESET_Pin GPIO_PIN_7
#define LCD_DISP_RESET_GPIO_Port GPIOH
#define RME_P3_Pin GPIO_PIN_5
#define RME_P3_GPIO_Port GPIOH
#define CTP_RST_Pin GPIO_PIN_3
#define CTP_RST_GPIO_Port GPIOE
#define RME_P2_Pin GPIO_PIN_4
#define RME_P2_GPIO_Port GPIOH
#define USR_LED_Pin GPIO_PIN_5
#define USR_LED_GPIO_Port GPIOE
#define RME_P1_Pin GPIO_PIN_10
#define RME_P1_GPIO_Port GPIOA
#define CTP_INT_Pin GPIO_PIN_6
#define CTP_INT_GPIO_Port GPIOE
#define CTP_INT_EXTI_IRQn EXTI6_IRQn
#define USB_PWR_EN_Pin GPIO_PIN_8
#define USB_PWR_EN_GPIO_Port GPIOG
#define USB_OVERCURRENT_Pin GPIO_PIN_0
#define USB_OVERCURRENT_GPIO_Port GPIOJ
#define TME_P4_Pin GPIO_PIN_2
#define TME_P4_GPIO_Port GPIOG
#define CME_P2_Pin GPIO_PIN_4
#define CME_P2_GPIO_Port GPIOI
#define R_RST_Pin GPIO_PIN_4
#define R_RST_GPIO_Port GPIOF
#define BUTTON_2_Pin GPIO_PIN_11
#define BUTTON_2_GPIO_Port GPIOH
#define CME_P3_Pin GPIO_PIN_0
#define CME_P3_GPIO_Port GPIOB
#define R_CS_Pin GPIO_PIN_1
#define R_CS_GPIO_Port GPIOG
#define TME_P1_Pin GPIO_PIN_10
#define TME_P1_GPIO_Port GPIOB
#define BUTTON_1_Pin GPIO_PIN_3
#define BUTTON_1_GPIO_Port GPIOA
#define BUTTON_3_Pin GPIO_PIN_6
#define BUTTON_3_GPIO_Port GPIOA
#define BUTTON_4_Pin GPIO_PIN_7
#define BUTTON_4_GPIO_Port GPIOA
#define R_INT_Pin GPIO_PIN_4
#define R_INT_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
