#ifndef STM32G4xx_HAL_H
#define STM32G4xx_HAL_H

#include <stdint.h>

#include "CCUStateData.h"
#include "stm32g4xx_ll_gpio.h"

typedef enum {
	GPIO_PIN_RESET = 0U,
	GPIO_PIN_SET
} GPIO_PinState;

uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickFreq(void);
uint32_t HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint32_t HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void SendPrechargeStatus(bool setPrecharge);
void SendDebugReport(char *data);

#endif
