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
			LOGOMATIC("CAN_send: currently in restricted operation mode\n");
			if (CubeMXCan_Private_RecoverPeripheral(handle) != HAL_OK) {
				LOGOMATIC("CAN_send %d: failed to recover peripheral\n", (int)i);
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

	uint32_t current_tail = atomic_load_explicit(&handle->tx_tail, memory_order_relaxed);
	uint32_t current_head = atomic_load_explicit(&handle->tx_head, memory_order_acquire);

	if ((current_tail - current_head) >= CUBEMX_CAN_TX_QUEUE_SIZE) {
		LOGOMATIC("CubeMXCan_QueueTx: queue is full\n");
		return HAL_TIMEOUT;
	}

	handle->tx_queue[current_tail & TX_QUEUE_MASK] = *message;
	atomic_store_explicit(&handle->tx_tail, current_tail + 1U, memory_order_release);
	return HAL_OK;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim != NULL && htim->Instance == CUBEMX_CAN_TIMER_INSTANCE) {
		CubeMXCan_Tick();
	}
}

HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		LOGOMATIC("CubeMXCan_Private_SendQueuedMessage: invalid null parameter\n");
		return HAL_ERROR;
	}

	uint32_t current_head = atomic_load_explicit(&handle->tx_head, memory_order_relaxed);
	uint32_t current_tail = atomic_load_explicit(&handle->tx_tail, memory_order_acquire);

	if (current_head == current_tail) {
		return HAL_OK;
	}

	GRCAN_TxMessage message_copy = handle->tx_queue[current_head & TX_QUEUE_MASK];

	if (HAL_FDCAN_AddMessageToTxFifoQ(handle->hfdcan, &message_copy.tx_header, message_copy.data) != HAL_OK) {
		return HAL_ERROR;
	}

	atomic_store_explicit(&handle->tx_head, (current_head + 1U), memory_order_release);
	return HAL_OK;
}
