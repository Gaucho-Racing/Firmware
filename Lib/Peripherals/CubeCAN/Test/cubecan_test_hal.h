#ifndef CUBECAN_TEST_HAL_H
#define CUBECAN_TEST_HAL_H

#include <stdbool.h>
#include <stdint.h>

#include "fdcan.h"

typedef struct {
	uint32_t activate_count;
	uint32_t deactivate_count;
	uint32_t start_count;
	uint32_t stop_count;
	uint32_t init_count;
	uint32_t restricted_exit_count;
	uint32_t error_handler_count;
	uint32_t tx_count;
	uint32_t rx_count;
	uint32_t last_interrupts;
	uint32_t last_tx_buffer;
	FDCAN_HandleTypeDef *tx_handles[32];
	FDCAN_TxHeaderTypeDef tx_headers[32];
	uint8_t tx_data[32][64];
	FDCAN_RxHeaderTypeDef rx_headers[32];
	uint8_t rx_data[32][64];
	uint8_t rx_sizes[32];
	uint32_t rx_count_pending;
	uint32_t tx_fifo_free_level;
	HAL_StatusTypeDef activate_status;
	HAL_StatusTypeDef deactivate_status;
	HAL_StatusTypeDef start_status;
	HAL_StatusTypeDef stop_status;
	HAL_StatusTypeDef init_status;
	HAL_StatusTypeDef tx_status;
	HAL_StatusTypeDef rx_status;
	HAL_StatusTypeDef protocol_status_result;
	HAL_StatusTypeDef restricted_exit_status;
	bool restricted_operation;
} CubeCAN_TestHal;

extern CubeCAN_TestHal cubecan_test_hal;

void CubeCAN_TestHal_Reset(void);
void CubeCAN_TestHal_QueueRx(const FDCAN_RxHeaderTypeDef *header, const uint8_t *data, uint8_t size);
void CubeCAN_TestHal_AdvanceTick(void);

#endif