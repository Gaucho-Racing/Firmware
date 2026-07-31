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

CubeMXCan_Handle *CubeMXCan_OneShotInitStart(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config)
{
	CubeMXCan_Handle *handle = CubeMXCan_Init(hfdcan, config);

	if (handle == NULL) {
		LOGOMATIC("CubeMXCan_OneShotInit: failed to initialize handle\n");
		return NULL;
	}

	if (CubeMXCan_Start(handle) != HAL_OK) {
		LOGOMATIC("CubeMXCan_OneShotInit: failed to start handle\n");
		CubeMXCan_Release(handle);
		return NULL;
	}

	return handle;
}

HAL_StatusTypeDef CubeMXCan_OneShotReleaseStop(CubeMXCan_Handle *handle)
{
	if (handle == NULL) {
		LOGOMATIC("CubeMXCan_OneShotReleaseStop: invalid null parameter\n");
		return NULL;
	}

	HAL_StatusTypeDef status = CubeMXCan_Stop(handle);

	if (status != HAL_OK) {
		LOGOMATIC("CubeMXCan_OneShotReleaseStop: failed to stop FDCAN instance\n");
		return status;
	}

	return CubeMXCan_Release(handle);
}

CubeMXCan_Handle *CubeMXCan_Init(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config)
{
	if (hfdcan == NULL || config == NULL) {
		LOGOMATIC("CubeMXCan_Init: invalid null parameters\n");
		return NULL;
	}

	CubeMXCan_Handle *handle = NULL;
	uint8_t free_handle_index = (uint8_t)-1;

	CRITICAL_SECTION
	{
		for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
			if (handles[i].hfdcan == hfdcan) {
				handle = &handles[i];
				break;
			} else if (handles[i].hfdcan == NULL && free_handle_index == (uint8_t)-1) {
				free_handle_index = i;
			}
		}

		if (handle != NULL) {
			LOGOMATIC("CubeMXCan_Init: handle already initialized for this FDCAN instance\n");
			return NULL;
		} else if (free_handle_index != (uint8_t)-1) {
			handle = &handles[free_handle_index];
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

	FDCAN_HandleTypeDef *hfdcan_to_stop = NULL;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL) {
			hfdcan_to_stop = handle->hfdcan;
			handle->hfdcan = NULL;
			handle->started = false;
		}
	}

	if (hfdcan_to_stop == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_FDCAN_Stop(hfdcan_to_stop);

	if (status == HAL_OK) {
		status = HAL_FDCAN_DeactivateNotification(hfdcan_to_stop, rx_events);
	}

	if (status != HAL_OK) {
		LOGOMATIC("CubeMXCan_Release: peripheral deactivation failed\n");

		CRITICAL_SECTION
		{
			handle->hfdcan = hfdcan_to_stop;
		}

		return status;
	}

	CRITICAL_SECTION
	{
		memset(handle, 0, sizeof(*handle));
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_Start(CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	bool dynamic_start = false;
	FDCAN_HandleTypeDef *hfdcan = NULL;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL && !handle->started) {
			hfdcan = handle->hfdcan;
			dynamic_start = true;
		}
	}

	if (!dynamic_start) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_FDCAN_Start(hfdcan);

	if (status == HAL_OK) {
		CRITICAL_SECTION
		{
			handle->started = true;
		}
	}

	return status;
}

HAL_StatusTypeDef CubeMXCan_Stop(CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	bool dynamic_stop = false;
	FDCAN_HandleTypeDef *hfdcan = NULL;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL && handle->started) {
			hfdcan = handle->hfdcan;
			dynamic_stop = true;
		}
	}

	if (!dynamic_stop) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_FDCAN_Stop(hfdcan);

	if (status == HAL_OK) {
		CRITICAL_SECTION
		{
			handle->started = false;
		}
	}

	return status;
}
