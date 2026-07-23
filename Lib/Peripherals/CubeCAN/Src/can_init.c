#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"
#include "main.h"

CubeMXCan_Handle *CubeMXCan_Init(FDCAN_HandleTypeDef *hfdcan, const CubeCAN_Config *config)
{
	if (hfdcan == NULL || config == NULL) {
		LOGOMATIC("CubeMXCan_Init: invalid null parameters\n");
		return NULL;
	}

	CubeMXCan_Handle *handle = NULL;
	CRITICAL_SECTION
	{
		for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
			if (handles[i].hfdcan == hfdcan || handles[i].hfdcan == NULL) {
				handle = &handles[i];
				break;
			}
		}

		if (handle != NULL) {
			memset(handle, 0, sizeof(*handle));
			handle->hfdcan = hfdcan;
			handle->config = *config;
			atomic_init(&handle->tx_head, 0U);
			atomic_init(&handle->tx_tail, 0U);
			handle->started = false;
		}
	}

	if (handle == NULL) {
		LOGOMATIC("CubeMXCan_Init: no free handle slots\n");
		return NULL;
	}

	uint32_t rx_events = FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_MESSAGE_LOST;
	if (HAL_FDCAN_ActivateNotification(hfdcan, rx_events, 0U) != HAL_OK) {
		LOGOMATIC("CubeMXCan_Init: failed to activate RX notifications\n");
		memset(handle, 0, sizeof(*handle));
		return NULL;
	}

	return handle;
}

HAL_StatusTypeDef CubeMXCan_Release(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return HAL_ERROR;
	}

	if (CubeMXCan_Stop(handle) != HAL_OK) {
		LOGOMATIC("CubeMXCan_Release: failed to stop CAN peripheral\n");
		return HAL_ERROR;
	}

	CRITICAL_SECTION
	{
		memset(handle, 0, sizeof(*handle));
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_Start(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return HAL_ERROR;
	}

	if (HAL_FDCAN_Start(handle->hfdcan) != HAL_OK) {
		return HAL_ERROR;
	}

	CRITICAL_SECTION
	{
		handle->started = true;
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_Stop(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return HAL_ERROR;
	}

	if (HAL_FDCAN_Stop(handle->hfdcan) != HAL_OK) {
		return HAL_ERROR;
	}

	CRITICAL_SECTION
	{
		handle->started = false;
	}

	return HAL_OK;
}
