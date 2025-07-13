/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

extern ADC_HandleTypeDef hadc2;

/* USER CODE BEGIN Private defines */

#define ADC_MAX 0x0FFF
#define ADC_CONV 0.0005f

typedef struct {
    // ADC1
    float AUX_SIGNAL;
    float BSPD_SIGNAL;
    float APPS1_SIGNAL;
    float APPS2_SIGNAL;
    float BRAKE_R_SIGNAL;
    float BRAKE_F_SIGNAL;
    // ADC2
    float IMD_SENSE;
    float AMS_SENSE;
    float BSE_SIGNAL;
    float BSPD_SENSE;
    float STEERING_ANGLE;
} AnalogInput;

extern AnalogInput globalAnalog;

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

/* USER CODE BEGIN Prototypes */
typedef enum {
    // ADC1
    AUX_SIGNAL = 0,
    BSPD_SIGNAL,
    APPS1_SIGNAL,
    APPS2_SIGNAL,
    BRAKE_R_SIGNAL,
    BRAKE_F_SIGNAL,
    // ADC2
    IMD_SENSE,
    AMS_SENSE,
    BSE_SIGNAL, // Who knows
    BSPD_SENSE,
    STEERING_ANGLE,
} AnalogSignal;

void updateAnalogInputs(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

