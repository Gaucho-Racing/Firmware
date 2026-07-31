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

CubeMXCan_Handle *CubeMXCan_Init(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config)
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

	if (HAL_FDCAN_ActivateNotification(hfdcan, rx_events, 0U) != HAL_OK) {
		LOGOMATIC("CubeMXCan_Init: failed to activate RX notifications\n");
		CRITICAL_SECTION
		{
			memset(handle, 0, sizeof(*handle));
		}
		return NULL;
	}

	return handle;
}

HAL_StatusTypeDef CubeMXCan_Release(CubeMXCan_Handle *handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_OK;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL) {
			status = HAL_FDCAN_Stop(handle->hfdcan);

			if (status == HAL_OK) {
				status = HAL_FDCAN_DeactivateNotification(handle->hfdcan, rx_events);
			}
			if (status == HAL_OK) {
				memset(handle, 0, sizeof(*handle));
			} else {
				LOGOMATIC("CubeMXCan_Release: peripheral deactivation failed\n");
			}

			return status;
		} else {
			return HAL_ERROR;
		}
	}
}

HAL_StatusTypeDef CubeMXCan_Start(CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_ERROR;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL && !handle->started) {
			status = HAL_FDCAN_Start(handle->hfdcan);
			if (status == HAL_OK) {
				handle->started = true;
			}
		}
	}

	return status;
}

HAL_StatusTypeDef CubeMXCan_Stop(CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_ERROR;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL && handle->started) {
			status = HAL_FDCAN_Stop(handle->hfdcan);
			if (status == HAL_OK) {
				handle->started = false;
			}
		}
	}

	return status;
}
