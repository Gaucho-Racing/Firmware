#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
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

HAL_StatusTypeDef CubeMXCan_Send(CubeMXCan_Handle *const handle, const GRCAN_NODE_ID rx_node, const GRCAN_MSG_ID msg_id, const void *const data, const uint8_t size)
{
	if (handle == NULL || data == NULL || size > FDCAN_MAX_DATA_BYTES) {
		return HAL_ERROR;
	}

	const uint32_t dlc = CubeMXCan_Private_BytesToDlc(size);
	if (dlc == FDCAN_DLC_BYTES_0 && size != 0) {
		LOGOMATIC("CubeMXCan_Send: invalid data length code\n");
		return HAL_ERROR;
	}

	uint32_t fdformat = 0;
	uint32_t brs = 0;
	switch (handle->hfdcan->Init.FrameFormat) {
		case FDCAN_FRAME_CLASSIC:
			fdformat = FDCAN_CLASSIC_CAN;
			brs = FDCAN_BRS_OFF;
			break;
		case FDCAN_FRAME_FD_NO_BRS:
			fdformat = FDCAN_FD_CAN;
			brs = FDCAN_BRS_OFF;
			break;
		case FDCAN_FRAME_FD_BRS:
			fdformat = FDCAN_FD_CAN;
			brs = FDCAN_BRS_ON;
			break;
		default:
			LOGOMATIC("CubeMXCan_Send: unsupported frame format\n");
			return HAL_ERROR;
	}

	const CAN_Identifier identifier_struct = {.tx_node_id = handle->config.sending_node_id, .rx_node_id = rx_node, .msg_id = msg_id};

	const FDCAN_TxHeaderTypeDef header = {.BitRateSwitch = brs,
					      .DataLength = CubeMXCan_Private_BytesToDlc(size),
					      .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
					      .FDFormat = fdformat,
					      .Identifier = Construct_CAN_Identifier(&identifier_struct),
					      .IdType = FDCAN_EXTENDED_ID,
					      .MessageMarker = 0U, // TODO We can do cool things with this to track transmission queue statistics
					      .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
					      .TxFrameType = FDCAN_DATA_FRAME};

	GRCAN_Private_TxMessage message = {.tx_header = header};
	memcpy(message.data, data, size);

	return CubeMXCan_Private_QueueTx(handle, &message);
}

HAL_StatusTypeDef CubeMXCan_Private_QueueTx(CubeMXCan_Handle *handle, const GRCAN_Private_TxMessage *message)
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

	const GRCAN_Private_TxMessage *const message_ptr = &handle->tx_queue[current_head & TX_QUEUE_MASK];

	if (HAL_FDCAN_AddMessageToTxFifoQ(handle->hfdcan, &message_ptr->tx_header, message_ptr->data) != HAL_OK) {
		return HAL_ERROR;
	}

	atomic_store_explicit(&handle->tx_head, (current_head + 1U), memory_order_release);
	return HAL_OK;
}
