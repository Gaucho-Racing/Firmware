#include <stddef.h>
#include <stdint.h>

#ifndef CUBEVCP_TEST_MAIN_H
#define CUBEVCP_TEST_MAIN_H

#define __weak __attribute__((weak))

typedef enum {
	HAL_OK = 0x00,
	HAL_ERROR = 0x01,
	HAL_BUSY = 0x02,
	HAL_TIMEOUT = 0x03
} HAL_StatusTypeDef;

typedef enum {
	HAL_UART_STATE_RESET = 0x00U,
	HAL_UART_STATE_READY = 0x01U
} HAL_UART_StateTypeDef;

typedef struct {
	uint32_t Instance;
	uint16_t TxXferSize;
	HAL_UART_StateTypeDef RxState;
} UART_HandleTypeDef;

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#define __HAL_UART_CLEAR_OREFLAG(huart) ((void)(huart))
#define __HAL_UART_CLEAR_NEFLAG(huart) ((void)(huart))
#define __HAL_UART_CLEAR_FEFLAG(huart) ((void)(huart))

#endif
