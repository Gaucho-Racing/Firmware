#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "CriticalSection.h"
#include "CubeCAN.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"

CubeCAN_Handle *CubeCAN_Entrance(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config)
{
	CubeCAN_Handle *handle = CubeCAN_Private_Init(hfdcan, config);

	if (handle == NULL) {
		LOGOMATIC_ERROR("CubeCAN_Entrance: failed to initialize handle\n");
		return NULL;
	}

	if (CubeCAN_Private_Start(handle) != HAL_OK) {
		LOGOMATIC_ERROR("CubeCAN_Entrance: failed to start handle\n");
		CubeCAN_Private_Release(handle);
		return NULL;
	}

	return handle;
}

HAL_StatusTypeDef CubeCAN_Exit(CubeCAN_Handle *handle)
{
	if (handle == NULL) {
		LOGOMATIC_ERROR("CubeCAN_Exit: invalid null parameter\n");
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = CubeCAN_Private_Stop(handle);

	if (status != HAL_OK) {
		LOGOMATIC_ERROR("CubeCAN_Exit: failed to stop FDCAN instance\n");
		return status;
	}

	return CubeCAN_Private_Release(handle);
}

CubeCAN_Handle *CubeCAN_Private_Init(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config)
{
	if (hfdcan == NULL || config == NULL) {
		LOGOMATIC_ERROR("CubeCAN_Private_Init: invalid null parameters\n");
		return NULL;
	}

	CubeCAN_Handle *handle = NULL;
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
			LOGOMATIC_WARNING("CubeCAN_Private_Init: handle already initialized for this FDCAN instance\n");
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
		LOGOMATIC_ERROR("CubeCAN_Private_Init: no free handle slots\n");
		return NULL;
	}

	if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_EVENTS, 0U) != HAL_OK) {
		LOGOMATIC_ERROR("CubeCAN_Private_Init: failed to activate RX notifications\n");
		CRITICAL_SECTION
		{
			memset(handle, 0, sizeof(*handle));
		}
		return NULL;
	}

	return handle;
}

HAL_StatusTypeDef CubeCAN_Private_Release(CubeCAN_Handle *handle)
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
		status = HAL_FDCAN_DeactivateNotification(hfdcan_to_stop, FDCAN_IT_RX_EVENTS);
	}

	if (status != HAL_OK) {
		LOGOMATIC_ERROR("CubeCAN_Release: peripheral deactivation failed\n");

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

HAL_StatusTypeDef CubeCAN_Private_Start(CubeCAN_Handle *const handle)
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

HAL_StatusTypeDef CubeCAN_Private_Stop(CubeCAN_Handle *const handle)
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
