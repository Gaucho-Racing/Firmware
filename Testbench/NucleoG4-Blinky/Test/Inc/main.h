#include <stdint.h>

#ifndef MAIN_H
#define MAIN_H

typedef enum {
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
	HAL_BUSY = 0x02U,
	HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef enum {
	GPIO_PIN_RESET = 0U,
	GPIO_PIN_SET
} GPIO_PinState;

#define USER_LED_Pin 4
#define USER_LED_GPIO_Port 42

void HAL_Delay(uint32_t Delay);
void HAL_GPIO_WritePin(uint32_t GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif
