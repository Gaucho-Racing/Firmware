#include <stdint.h>

#include "main.h"

#ifndef CUBEVCP_TEST_HAL_H
#define CUBEVCP_TEST_HAL_H

typedef struct {
	uint32_t transmit_count;
	uint32_t receive_count;
	uint8_t transmit_data[64];
	uint16_t transmit_size;
	uint8_t *receive_buffer;
	uint16_t receive_size;
	HAL_StatusTypeDef transmit_status;
	HAL_StatusTypeDef receive_status;
} CubeVCP_TestHal;

extern CubeVCP_TestHal cubevcp_test_hal;

void CubeVCP_TestHal_Reset(void);

#endif
