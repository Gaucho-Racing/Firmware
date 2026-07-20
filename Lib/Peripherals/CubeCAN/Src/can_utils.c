#include <stdint.h>

#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"

static const uint8_t s_dlc_to_bytes[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};

const char *CAN_GetInstanceName(FDCAN_GlobalTypeDef *instance)
{
	if (instance == NULL) {
		return "NULL";
	}
#ifdef USECAN1
	if (instance == FDCAN1) {
		return "FDCAN1";
	}
#endif
#ifdef USECAN2
	if (instance == FDCAN2) {
		return "FDCAN2";
	}
#endif
#ifdef USECAN3
	if (instance == FDCAN3) {
		return "FDCAN3";
	}
#endif
	LOGOMATIC("Get CAN instance name: unknown instance\n");
	return "UNKNOWN";
}

uint32_t Construct_CAN_Identifier(const CAN_Identifier *identifier)
{
	if (identifier == NULL) {
		return 0U;
	}

	uint32_t message_id = 0U;
	message_id |= ((uint32_t)(identifier->tx_node_id & 0xFFU) << 20U);
	message_id |= ((uint32_t)(identifier->msg_id & 0xFFFU) << 8U);
	message_id |= (uint32_t)(identifier->rx_node_id & 0xFFU);

	return message_id;
}

CAN_Identifier Deconstruct_CAN_Identifier(uint32_t message_id)
{
	CAN_Identifier identifier = {0};

	identifier.tx_node_id = (GRCAN_NODE_ID)((message_id >> 20U) & 0xFFU);
	identifier.msg_id = (GRCAN_MSG_ID)((message_id >> 8U) & 0xFFFU);
	identifier.rx_node_id = (GRCAN_NODE_ID)(message_id & 0xFFU);

	return identifier;
}

HAL_StatusTypeDef CubeCANExt_BuildExtendedFilter(const CAN_Identifier *identifier, uint32_t filter_index, uint32_t fifo, FDCAN_FilterTypeDef *filter)
{
	if (identifier == NULL || filter == NULL) {
		return HAL_ERROR;
	}

	filter->IdType = FDCAN_EXTENDED_ID;
	filter->FilterIndex = filter_index;
	filter->FilterType = FDCAN_FILTER_MASK;
	filter->FilterConfig = fifo;
	filter->FilterID1 = Construct_CAN_Identifier(identifier);
	filter->FilterID2 = 0x1FFFFFFFU;

	return HAL_OK;
}

uint8_t CubeMXCan_Private_DlcToBytes(uint32_t dlc)
{
	if (dlc > 15U) {
		return 64U;
	}

	return s_dlc_to_bytes[dlc];
}

bool CubeMXCan_Private_IsDisabled(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return true;
	}

	HAL_FDCAN_StateTypeDef state = HAL_FDCAN_GetState(handle->hfdcan);
	if (state == HAL_FDCAN_STATE_ERROR || state == HAL_FDCAN_STATE_READY) {
		return true;
	}

	uint32_t error_status = HAL_FDCAN_GetErrorStatus(handle->hfdcan);
	if ((error_status & FDCAN_PSR_BO) != 0U) { // BO = Bus-Off Flag
		return true;
	}

	return false;
}

HAL_StatusTypeDef CubeMXCan_Private_RecoverPeripheral(CubeMXCan_Handle *handle)
{
	FDCAN_ProtocolStatusTypeDef protocol_status = {0};

	if (HAL_FDCAN_GetProtocolStatus(handle->hfdcan, &protocol_status) == HAL_OK && protocol_status.BusOff) {
		LOGOMATIC("CAN_send: bus off detected, attempting recovery\n");

		if (HAL_FDCAN_Stop(handle->hfdcan) != HAL_OK) {
			LOGOMATIC("CAN_send: failed to stop FDCAN peripheral during bus off recovery\n");
			return HAL_ERROR;
		}

		HAL_FDCAN_AbortTxRequest(handle->hfdcan, FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2);

		if (HAL_FDCAN_Start(handle->hfdcan) != HAL_OK) {
			LOGOMATIC("CAN_send: failed to restart FDCAN peripheral during bus off recovery\n");
			return HAL_ERROR;
		}
	}

	if (HAL_FDCAN_IsRestrictedOperationMode(handle->hfdcan)) {
		LOGOMATIC("CAN_send: currently in restricted operation mode\n");
		HAL_FDCAN_ExitRestrictedOperationMode(handle->hfdcan);
	}

	return HAL_OK;
}

HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		// Invalid parameter
		return HAL_ERROR;
	}

	uint32_t current_head = atomic_load_explicit(&handle->tx_head, memory_order_relaxed);
	uint32_t current_tail = atomic_load_explicit(&handle->tx_tail, memory_order_acquire);

	if (current_head == current_tail) {
		// Queue empty
		return HAL_OK;
	}

	uint32_t current_index = current_head & TX_QUEUE_MASK;
	GRCAN_TxMessage message_copy = handle->tx_queue[current_index];

	if (HAL_FDCAN_AddMessageToTxFifoQ(handle->hfdcan, &message_copy.tx_header, message_copy.data) != HAL_OK) {
		return HAL_ERROR;
	}

	atomic_store_explicit(&handle->tx_head, (current_head + 1U), memory_order_release);
	return HAL_OK;
}
