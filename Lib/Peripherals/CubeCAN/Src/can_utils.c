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

HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(CubeMXCan_Handle *handle)
{
	if (handle == NULL || handle->hfdcan == NULL) {
		return HAL_ERROR;
	}

	GRCAN_TxMessage message_copy;
	bool message_found = false;

	CRITICAL_SECTION
	{
		if (handle->tx_count > 0U) {
			message_copy = handle->tx_queue[handle->tx_head];
			message_found = true;
		}
	}

	if (!message_found) {
		return HAL_ERROR;
	}

	if (HAL_FDCAN_AddMessageToTxFifoQ(handle->hfdcan, &message_copy.tx_header, message_copy.data) != HAL_OK) {
		return HAL_ERROR;
	}

	CRITICAL_SECTION
	{
		if (handle->tx_count > 0U) {
			handle->tx_head = (handle->tx_head + 1U) % CUBEMX_CAN_TX_QUEUE_SIZE;
			handle->tx_count--;
		}
	}

	return HAL_OK;
}
