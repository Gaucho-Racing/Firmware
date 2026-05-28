/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    timer.h
 * @brief   This file contains all the function prototypes for
 *          the timer.c file
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
#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */

void WHEEL_SPEED_TIMER_INIT(TIM_HandleTypeDef *handle, uint32_t period_multiple_of_100us);



/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ TIMER_H__ */
