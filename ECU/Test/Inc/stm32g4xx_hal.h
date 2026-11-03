#ifndef STM32G4xx_HAL_H
#define STM32G4xx_HAL_H

#include <stdint.h>

typedef enum {
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
	HAL_BUSY = 0x02U,
	HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;

typedef struct FDCAN_HandleTypeDef FDCAN_HandleTypeDef;

uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickFreq(void);

#endif
