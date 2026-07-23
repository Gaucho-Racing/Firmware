#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"

#define CAN_TX_NODE_SHIFT 20U
#define CAN_MSG_SHIFT 8U
#define CAN_RX_NODE_SHIFT 0U
#define CAN_NODE_MASK 0xFFU
#define CAN_MSG_MASK 0xFFFU

uint32_t Construct_CAN_Identifier(const CAN_Identifier *identifier)
{
	if (identifier == NULL) {
		return 0U;
	}

	return // Transmitting node
	    ((uint32_t)(identifier->tx_node_id & CAN_NODE_MASK) << CAN_TX_NODE_SHIFT)
	    // Message ID
	    | ((uint32_t)(identifier->msg_id & CAN_MSG_MASK) << CAN_MSG_SHIFT)
	    // Receiving node
	    | ((uint32_t)(identifier->rx_node_id & CAN_NODE_MASK) << CAN_RX_NODE_SHIFT);
}

CAN_Identifier Deconstruct_CAN_Identifier(uint32_t message_id)
{
	return (CAN_Identifier){// Transmitting node
				.tx_node_id = (GRCAN_NODE_ID)((message_id >> CAN_TX_NODE_SHIFT) & CAN_NODE_MASK),
				// Message ID
				.msg_id = (GRCAN_MSG_ID)((message_id >> CAN_MSG_SHIFT) & CAN_MSG_MASK),
				// Receiving node
				.rx_node_id = (GRCAN_NODE_ID)(message_id >> CAN_RX_NODE_SHIFT) & CAN_NODE_MASK};
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

HAL_StatusTypeDef CubeMXCan_AddFilter(CubeMXCan_Handle *handle, const FDCAN_FilterTypeDef *filter)
{
	if (handle == NULL || handle->hfdcan == NULL || filter == NULL) {
		return HAL_ERROR;
	}

	return HAL_FDCAN_ConfigFilter(handle->hfdcan, filter);
}

uint8_t CubeMXCan_Private_DlcToBytes(uint32_t dlc)
{
	static const uint8_t dlc_to_bytes[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};

	static_assert((sizeof(dlc_to_bytes) / sizeof(dlc_to_bytes[0])) == 16, "dlc_to_bytes array must have 16 elements");

	return (dlc > 15U) ? 64U : dlc_to_bytes[dlc];
}

bool CubeMXCan_Private_IsDisabled(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return true;
	}

	HAL_FDCAN_StateTypeDef state = HAL_FDCAN_GetState(handle->hfdcan);
	FDCAN_ProtocolStatusTypeDef protocol_status = {0};

	return // HAL states
	    state == HAL_FDCAN_STATE_ERROR ||
	    state == HAL_FDCAN_STATE_READY
	    // Check for bus off condition
	    || (HAL_FDCAN_GetProtocolStatus(handle->hfdcan, &protocol_status) == HAL_OK && protocol_status.BusOff);
}

HAL_StatusTypeDef CubeMXCan_Private_RecoverPeripheral(CubeMXCan_Handle *handle)
{
	FDCAN_ProtocolStatusTypeDef protocol_status = {0};

	if (HAL_FDCAN_GetProtocolStatus(handle->hfdcan, &protocol_status) == HAL_OK && protocol_status.BusOff) {
		LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: bus off detected, attempting recovery\n");

		if (HAL_FDCAN_Stop(handle->hfdcan) != HAL_OK) {
			LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: failed to stop FDCAN peripheral during bus off recovery\n");
			return HAL_ERROR;
		}

		HAL_FDCAN_AbortTxRequest(handle->hfdcan, FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2);

		if (HAL_FDCAN_Start(handle->hfdcan) != HAL_OK) {
			LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: failed to restart FDCAN peripheral during bus off recovery\n");
			return HAL_ERROR;
		}
	}

	if (HAL_FDCAN_IsRestrictedOperationMode(handle->hfdcan)) {
		LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: currently in restricted operation mode\n");
		HAL_FDCAN_ExitRestrictedOperationMode(handle->hfdcan);
	}

	return HAL_OK;
}
