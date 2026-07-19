#include <stdbool.h>
#include <string.h>

#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"

typedef struct {
	FDCAN_GlobalTypeDef *instance;
	CubeMXCan_Handle *handle;
} CubeMXCan_RegistryEntry;

static CubeMXCan_RegistryEntry s_registry[CUBEMX_CAN_MAX_INSTANCES] = {0};
static CubeMXCan_Handle s_handles[CUBEMX_CAN_MAX_INSTANCES] = {0};
static bool s_timer_started = false;

static CubeMXCan_RegistryEntry *CubeMXCan_FindRegistryEntry(FDCAN_GlobalTypeDef *instance)
{
	for (uint32_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		if (s_registry[i].instance == instance) {
			return &s_registry[i];
		}
	}

	return NULL;
}

static CubeMXCan_RegistryEntry *CubeMXCan_AllocRegistryEntry(FDCAN_GlobalTypeDef *instance)
{
	CubeMXCan_RegistryEntry *entry = CubeMXCan_FindRegistryEntry(instance);
	if (entry != NULL) {
		return entry;
	}

	for (uint32_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		if (s_registry[i].instance == NULL) {
			s_registry[i].instance = instance;
			return &s_registry[i];
		}
	}

	return NULL;
}

CubeMXCan_Handle *CubeMXCan_Private_GetHandle(FDCAN_HandleTypeDef *hfdcan)
{
	if (hfdcan == NULL) {
		return NULL;
	}

	CubeMXCan_RegistryEntry *entry = CubeMXCan_FindRegistryEntry(hfdcan->Instance);
	return (entry != NULL) ? entry->handle : NULL;
}

HAL_StatusTypeDef CubeMXCan_Private_RegisterHandle(FDCAN_HandleTypeDef *hfdcan, CubeMXCan_Handle *handle)
{
	if (hfdcan == NULL || handle == NULL) {
		return HAL_ERROR;
	}

	CubeMXCan_RegistryEntry *entry = CubeMXCan_AllocRegistryEntry(hfdcan->Instance);
	if (entry == NULL) {
		return HAL_ERROR;
	}

	entry->handle = handle;
	return HAL_OK;
}

void CubeMXCan_Private_UnregisterHandle(FDCAN_HandleTypeDef *hfdcan)
{
	if (hfdcan == NULL) {
		return;
	}

	CubeMXCan_RegistryEntry *entry = CubeMXCan_FindRegistryEntry(hfdcan->Instance);
	if (entry != NULL) {
		entry->instance = NULL;
		entry->handle = NULL;
	}
}

void CubeMXCan_Tick(void)
{
	for (uint32_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		CubeMXCan_Handle *handle = s_registry[i].handle;
		if (handle == NULL || !handle->started) {
			continue;
		}

		(void)CubeMXCan_Private_SendQueuedMessage(handle);
	}
}

CubeMXCan_Handle *CubeMXCan_Init(FDCAN_HandleTypeDef *hfdcan, const CubeCAN_Config *config)
{
	if (hfdcan == NULL || config == NULL) {
		return NULL;
	}

	CubeMXCan_Handle *handle = NULL;
	bool register_ok = false;

	CRITICAL_SECTION
	{
		handle = CubeMXCan_Private_GetHandle(hfdcan);
		if (handle == NULL) {
			for (uint32_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
				if (s_handles[i].hfdcan == NULL) {
					handle = &s_handles[i];
					memset(handle, 0, sizeof(*handle));
					handle->hfdcan = hfdcan;
					break;
				}
			}
		}

		if (handle != NULL) {
			handle->config = *config;
			handle->tx_head = 0U;
			handle->tx_count = 0U;
			handle->started = false;

			if (CubeMXCan_Private_RegisterHandle(hfdcan, handle) == HAL_OK) {
				register_ok = true;
			} else {
				memset(handle, 0, sizeof(*handle));
				handle = NULL;
			}
		}
	}

	if (handle == NULL) {
		LOGOMATIC("CubeMXCan_Init: no free handle slots\n");
		return NULL;
	}

	if (!register_ok) {
		LOGOMATIC("CubeMXCan_Init: failed to register handle\n");
		return NULL;
	}

	uint32_t rx_events = FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_MESSAGE_LOST;
	if (HAL_FDCAN_ActivateNotification(hfdcan, rx_events, 0U) != HAL_OK) {
		LOGOMATIC("CubeMXCan_Init: failed to activate RX notifications\n");
		return NULL;
	}

	if (!s_timer_started) {
		s_timer_started = true;
	}
	return handle;
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

	CRITICAL_SECTION
	{
		handle->started = false;
	}

	if (HAL_FDCAN_Stop(handle->hfdcan) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_Release(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return HAL_ERROR;
	}

	(void)CubeMXCan_Stop(handle);
	
	CRITICAL_SECTION
	{
		CubeMXCan_Private_UnregisterHandle(handle->hfdcan);
		memset(handle, 0, sizeof(*handle));
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_AddFilter(CubeMXCan_Handle *handle, const FDCAN_FilterTypeDef *filter)
{
	if (handle == NULL || handle->hfdcan == NULL || filter == NULL) {
		return HAL_ERROR;
	}

	return HAL_FDCAN_ConfigFilter(handle->hfdcan, (FDCAN_FilterTypeDef *)filter);
}

HAL_StatusTypeDef CubeMXCan_QueueTx(CubeMXCan_Handle *handle, const GRCAN_TxMessage *message)
{
	if (handle == NULL || message == NULL) {
		return HAL_ERROR;
	}

	CRITICAL_SECTION
	{
		uint32_t insert_index = (handle->tx_head + handle->tx_count) % CUBEMX_CAN_TX_QUEUE_SIZE;
		handle->tx_queue[insert_index] = *message;

		if (handle->tx_count < CUBEMX_CAN_TX_QUEUE_SIZE) {
			handle->tx_count++;
		} else {
			handle->tx_head = (handle->tx_head + 1U) % CUBEMX_CAN_TX_QUEUE_SIZE;
		}
	}

	return HAL_OK;
}
