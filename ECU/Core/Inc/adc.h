/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    adc.h
 * @brief   This file contains all the function prototypes for
 *          the adc.c file
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
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define ADC_MAX 0x0FFF
#define ADC_CONV 0.0005f

/* USER CODE END Private defines */

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

/* USER CODE BEGIN Prototypes */

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

extern AnalogInput globalAnalog;

const uint32_t ADC1_COUNT = 6;
unsigned long ADC1_pins[ADC1_COUNT] = {BSPD_SIGNAL_Pin,	 APPS1_SIGNAL_Pin,
			     APPS2_SIGNAL_Pin,	 BRAKE_F_SIGNAL_Pin,
			     BRAKE_R_SIGNAL_Pin, AUX_SIGNAL_Pin};
unsigned long ADC1_ports[ADC1_COUNT] = {BSPD_SIGNAL_GPIO_Port, APPS1_SIGNAL_GPIO_Port, APPS2_SIGNAL_GPIO_Port, BRAKE_F_SIGNAL_GPIO_Port, BRAKE_R_SIGNAL_GPIO_Port, AUX_SIGNAL_GPIO_Port};

const uint32_t ADC2_COUNT = 5;
unsigned long ADC2_pins[ADC2_COUNT] = {BSE_SIGNAL_Pin, BSPD_SENSE_Pin, IMD_SENSE_Pin,
			     AMS_SENSE_Pin, STEERING_ANGLE_Pin};
unsigned long ADC2_ports[ADC2_COUNT] = {BSE_SIGNAL_GPIO_Port, BSPD_SENSE_GPIO_Port, IMD_SENSE_GPIO_Port, AMS_SENSE_GPIO_Port, STEERING_ANGLE_GPIO_Port};

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
