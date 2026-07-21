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

	(void)CubeMXCan_Stop(handle);

	CRITICAL_SECTION
	{
		memset(handle, 0, sizeof(*handle));
	}

	return HAL_OK;
}

void CubeMXCan_Tick(void)
{
	for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		CubeMXCan_Handle *handle = registry[i].handle;

		CRITICAL_SECTION
		{
			if (handle == NULL || handle->hfdcan == NULL || !handle->started) {
				continue;
			}
		}

		if (CubeMXCan_Private_IsDisabled(handle)) {
			LOGOMATIC("CAN_send: currently in restricted operation mode\n");
			if (CubeMXCan_Private_RecoverPeripheral(handle) != HAL_OK) {
				LOGOMATIC("CAN_send %d: failed to recover peripheral\n", (int)i);
				continue;
			}
		}

		(void)CubeMXCan_Private_SendQueuedMessage(handle);
	}
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

	uint32_t current_tail = atomic_load(&handle->tx_tail);
	uint32_t current_head = atomic_load(&handle->tx_head);

	if ((current_tail - current_head) >= CUBEMX_CAN_TX_QUEUE_SIZE) {
		LOGOMATIC("CubeMXCan_QueueTx: queue is full\n");
		return HAL_TIMEOUT;
	}

	uint32_t insert_index = current_tail & TX_QUEUE_MASK;
	handle->tx_queue[insert_index] = *message;

	atomic_store_explicit(&handle->tx_tail, current_tail + 1U, memory_order_release);

	return HAL_OK;
}
