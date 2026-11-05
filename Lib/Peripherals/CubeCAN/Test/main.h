#include <stdint.h>

#ifndef MAIN_H
#define MAIN_H

typedef struct FDCAN_HandleTypeDef FDCAN_HandleTypeDef;

typedef enum {
	HAL_OK = 0x00,
	HAL_ERROR = 0x01,
	HAL_BUSY = 0x02,
	HAL_TIMEOUT = 0x03
} HAL_StatusTypeDef;

uint32_t HAL_GetTick(void);

void Error_Handler(void);

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

#endif
