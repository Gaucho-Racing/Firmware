#include <stdbool.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"
#include "main.h"

void CubeMXCan_Tick(void)
{
	for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		CubeMXCan_Handle *handle = &handles[i];

		if (handle->hfdcan == NULL || !handle->started) {
			continue;
		}

		if (CubeMXCan_Private_IsDisabled(handle)) {
			LOGOMATIC("CubeMXCan_Tick: currently in restricted operation mode\n");
			if (CubeMXCan_Private_RecoverPeripheral(handle) != HAL_OK) {
				LOGOMATIC("CubeMXCan_Tick %d: failed to recover peripheral\n", (int)i);
				continue;
			}
		}

		(void)CubeMXCan_Private_SendQueuedMessage(handle);
	}
}

HAL_StatusTypeDef CubeMXCan_QueueTx(CubeMXCan_Handle *handle, const GRCAN_TxMessage *message)
{
	if (handle == NULL || message == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_OK;

	CRITICAL_SECTION
	{
		const uint32_t tail = atomic_load_explicit(&handle->tx_tail, memory_order_relaxed);
		uint32_t head = atomic_load_explicit(&handle->tx_head, memory_order_relaxed);

		if ((tail - head) >= CUBEMX_CAN_TX_QUEUE_SIZE) {
			head++; // Drop oldest message to make room for the new one
			atomic_store_explicit(&handle->tx_head, head, memory_order_relaxed);
			status = HAL_BUSY;
		}
		handle->tx_queue[tail & TX_QUEUE_MASK] = *message;

		atomic_store_explicit(&handle->tx_tail, tail + 1U, memory_order_release);
	}

	return status;
}

HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(const CubeMXCan_Handle *const handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		LOGOMATIC("CubeMXCan_Private_SendQueuedMessage: invalid null parameter\n");
		return HAL_ERROR;
	}

	if (HAL_FDCAN_GetTxFifoFreeLevel(handle->hfdcan) == 0U) {
		LOGOMATIC("CubeMXCan_Private_SendQueuedMessage: Tx FIFO full, cannot send message\n");
		return HAL_BUSY;
	}

	const uint32_t current_head = atomic_load_explicit(&handle->tx_head, memory_order_relaxed);
	const uint32_t current_tail = atomic_load_explicit(&handle->tx_tail, memory_order_acquire);

	if (current_head == current_tail) {
		return HAL_OK;
	}

	const GRCAN_TxMessage *const message_ptr = &handle->tx_queue[current_head & TX_QUEUE_MASK];

	if (HAL_FDCAN_AddMessageToTxFifoQ(handle->hfdcan, &message_ptr->tx_header, message_ptr->data) != HAL_OK) {
		return HAL_ERROR;
	}

	atomic_store_explicit(&handle->tx_head, (current_head + 1U), memory_order_release);
	return HAL_OK;
}
