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
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_smbus.h"
#include "stm32g4xx_hal_tim.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_utils.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define MLX90614_ADDRESS 0x5A
#define BRAKETEMP_INTERVAL_MS 33       // 30 Hz transmission speed (will be slightly slower in practice)
#define WHEEL_SPEED_TIMEOUT_TICKS 1000 // 1 second of no pulse means the car stopped moving
#define DEBOUNCE_NUM_10us 10	       // Multiple of 100 microseconds
#define MAX_RPM 1891		       // Assumes 8 inch wheels @ max speed of 90 mph
#define SECONDS_PER_MIN 60
#define PULSES_PER_ROT 6     // This is often the same as the number of teeth in the gear in front of the Hall Effect sensor
#define MAX_NUM_INTERVALS 32 // Must be a power of 2 and be storable in uint8_t
#define MIN_TICK_DELTA 100   // 100ms (somewhat arbitrary)
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern TIM_HandleTypeDef htim6;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* Size of buffer */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
