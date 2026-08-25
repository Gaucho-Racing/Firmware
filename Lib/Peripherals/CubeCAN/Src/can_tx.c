#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "CriticalSection.h"
#include "CubeCAN.h"
#include "CubeCAN_Config.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"

void CubeCAN_Private_RateChecker(void)
{
	static uint32_t last_tick = 0;
	static uint32_t call_count_in_tick = 0;
	static uint32_t violations = 0;

	const uint32_t current_tick = HAL_GetTick();

	if (current_tick == last_tick) {
		call_count_in_tick++;
	} else {
		last_tick = current_tick;
		call_count_in_tick = 1;
	}

	if (current_tick > 10U && call_count_in_tick > ABSOLUTE_MAX_INVOCATIONS_PER_TICK) {
		LOGOMATIC_ERROR("CubeCAN_Tick: called too frequently (%" PRIu32 " times in 1 ms), you NEED to fix your timers!\tViolation %" PRIu32 "\n", call_count_in_tick, ++violations);
#ifdef RELAXED_TIMER_GATE // Hidden feature flag if you REALLY know what you are doing but you should fix your timers instead
#warning "CubeCAN_Tick: Compiled with RELAXED_TIMER_GATE, this is a hidden feature flag that allows you to ignore the timer violation but you really should fix your timers"
#else
		Error_Handler();
#endif
	}
}

void CubeCAN_Tick(void)
{
#ifdef DISABLED_TIMER_GATE
#warning "CubeCAN_Tick: Compiled with DISABLED_TIMER_GATE, this is a hidden feature flag that disables the timer violation check but you really should fix your timers"
	static bool rate_checker_gate = true;
#else
	static bool rate_checker_gate = false;
#endif

	if (__builtin_expect(!rate_checker_gate, 0)) {
		CubeCAN_Private_RateChecker();

		if (HAL_GetTick() > 5000U) { // Disable check after 5 seconds
			rate_checker_gate = true;
		}
	}

	for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		CubeCAN_Handle *handle = &handles[i];

		if (handle->hfdcan == NULL || !handle->started) {
			continue;
		}

		if (CubeCAN_Private_IsDisabled(handle)) {
			LOGOMATIC_WARNING("CubeCAN_Tick: currently in restricted operation mode\n");
			if (CubeCAN_Private_RecoverPeripheral(handle) != HAL_OK) {
				LOGOMATIC_ERROR("CubeCAN_Tick %d: failed to recover peripheral\n", (int)i);
				continue;
			}
		}

		(void)CubeCAN_Private_SendQueuedMessage(handle);
	}
}

HAL_StatusTypeDef CubeCAN_Send(CubeCAN_Handle *const handle, const GRCAN_NODE_ID rx_node, const GRCAN_MSG_ID msg_id, const void *const data, const uint8_t size)
{
	if (handle == NULL || data == NULL || size > FDCAN_MAX_DATA_BYTES) {
		return HAL_ERROR;
	}

	const uint32_t dlc = CubeCAN_Private_BytesToDlc(size);
	if (dlc == FDCAN_DLC_BYTES_0 && size != 0) {
		LOGOMATIC_ERROR("CubeCAN_Send: invalid data length code\n");
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
			LOGOMATIC_ERROR("CubeCAN_Send: unsupported frame format\n");
			return HAL_ERROR;
	}

	const CAN_Identifier identifier_struct = {.tx_node_id = handle->config.sending_node_id, .rx_node_id = rx_node, .msg_id = msg_id};

	const FDCAN_TxHeaderTypeDef header = {.BitRateSwitch = brs,
					      .DataLength = CubeCAN_Private_BytesToDlc(size),
					      .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
					      .FDFormat = fdformat,
					      .Identifier = CubeCAN_Construct_Identifier(&identifier_struct),
					      .IdType = FDCAN_EXTENDED_ID,
					      .MessageMarker = 0U, // TODO We can do cool things with this to track transmission queue statistics
					      .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
					      .TxFrameType = FDCAN_DATA_FRAME};

	GRCAN_Private_TxMessage message = {.tx_header = header};
	memcpy(message.data, data, size);

	return CubeCAN_Private_QueueTx(handle, &message);
}

HAL_StatusTypeDef CubeCAN_Private_QueueTx(CubeCAN_Handle *handle, const GRCAN_Private_TxMessage *message)
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

HAL_StatusTypeDef CubeCAN_Private_SendQueuedMessage(const CubeCAN_Handle *const handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		LOGOMATIC_ERROR("CubeCAN_Private_SendQueuedMessage: invalid null parameter\n");
		return HAL_ERROR;
	}

	if (HAL_FDCAN_GetTxFifoFreeLevel(handle->hfdcan) == 0U) {
		LOGOMATIC_ERROR("CubeCAN_Private_SendQueuedMessage: Tx FIFO full, cannot send message\n");
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
