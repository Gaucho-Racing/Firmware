/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_NODE_ID.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_utils.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* ADC mappings from `ADC_buffers` and `ADC_outputs` arrays to signal/sense names */
enum {
	/* Accelerator Pedal Position Sensor 1 - Signal */
	ADC_BUFFER_SIG_APPS1 = 0,
	/* Brake Pressure - Signal */
	ADC_BUFFER_SIG_BRAKE = 1,
	/* Steering Angle - Signal */
	ADC_BUFFER_SIG_STEERING_ANGLE = 2,
	/* Brake System Plausibility Device - Signal */
	ADC_BUFFER_SIG_BSPD = 3,
	/* Brake System Plausibility Device - Sense */
	ADC_BUFFER_SENSE_BSPD = 4,

	/* Accelerator Pedal Position Sensor 2 - Signal */
	ADC_BUFFER_SIG_APPS2 = 5,
	/* Brake Signal Encoder - Signal */
	ADC_BUFFER_SIG_BSE = 6,
	/* Accumulator Management System - Sense */
	ADC_BUFFER_SENSE_BMS = 7,
	/* Auxiliary - Signal */
	ADC_BUFFER_SIG_AUX = 8,
	/* Spare - Signal */
	ADC_BUFFER_SIG_SPARE2 = 9,

	/* Insulation Monitoring Device - Sense */
	ADC_BUFFER_SENSE_IMD = 10,
};


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
#define BRAKE_LIGHT_Pin GPIO_PIN_14
#define BRAKE_LIGHT_GPIO_Port GPIOC
#define SPARE_1_SIGNAL_Pin GPIO_PIN_1
#define SPARE_1_SIGNAL_GPIO_Port GPIOF
#define BRAKE_SIGNAL_Pin GPIO_PIN_1
#define BRAKE_SIGNAL_GPIO_Port GPIOC
#define APPS1_SIGNAL_Pin GPIO_PIN_3
#define APPS1_SIGNAL_GPIO_Port GPIOC
#define STEERING_ANGLE_Pin GPIO_PIN_3
#define STEERING_ANGLE_GPIO_Port GPIOA
#define APPS2_SIGNAL_Pin GPIO_PIN_4
#define APPS2_SIGNAL_GPIO_Port GPIOA
#define SPARE_2_SIGNAL_Pin GPIO_PIN_7
#define SPARE_2_SIGNAL_GPIO_Port GPIOA
#define BSPD_SIGNAL_Pin GPIO_PIN_0
#define BSPD_SIGNAL_GPIO_Port GPIOB
#define BSE_SIGNAL_Pin GPIO_PIN_2
#define BSE_SIGNAL_GPIO_Port GPIOB
#define SOFTWARE_OK_CONTROL_Pin GPIO_PIN_11
#define SOFTWARE_OK_CONTROL_GPIO_Port GPIOB
#define BSPD_SENSE_Pin GPIO_PIN_14
#define BSPD_SENSE_GPIO_Port GPIOB
#define AMS_SENSE_Pin GPIO_PIN_15
#define AMS_SENSE_GPIO_Port GPIOB
#define IMD_SENSE_Pin GPIO_PIN_9
#define IMD_SENSE_GPIO_Port GPIOA
#define LED_TEST_Pin GPIO_PIN_15
#define LED_TEST_GPIO_Port GPIOA
#define TSSI_G_CONTROL_Pin GPIO_PIN_2
#define TSSI_G_CONTROL_GPIO_Port GPIOD
#define TSSI_R_CONTROL_Pin GPIO_PIN_4
#define TSSI_R_CONTROL_GPIO_Port GPIOB
#define RTD_CONTROL_Pin GPIO_PIN_7
#define RTD_CONTROL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define MAIN_LOOP_PERIOD_US 10000
#define CAN_TIMER_SEND_PERIOD_US (MAIN_LOOP_PERIOD_US >> 4)

/* Private defines -----------------------------------------------------------*/

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
