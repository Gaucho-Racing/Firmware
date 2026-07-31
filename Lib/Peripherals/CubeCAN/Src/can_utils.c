#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"

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

	const uint32_t tx_node_id = identifier->tx_node_id & CAN_NODE_MASK;
	const uint32_t msg_id = identifier->msg_id & CAN_MSG_MASK;
	const uint32_t rx_node_id = identifier->rx_node_id & CAN_NODE_MASK;

	return (tx_node_id << CAN_TX_NODE_SHIFT) | (msg_id << CAN_MSG_SHIFT) | (rx_node_id << CAN_RX_NODE_SHIFT);
}

CAN_Identifier Deconstruct_CAN_Identifier(const uint32_t message_id)
{
	const GRCAN_NODE_ID tx_node_id = (message_id >> CAN_TX_NODE_SHIFT) & CAN_NODE_MASK;
	const GRCAN_MSG_ID msg_id = (message_id >> CAN_MSG_SHIFT) & CAN_MSG_MASK;
	const GRCAN_NODE_ID rx_node_id = (message_id >> CAN_RX_NODE_SHIFT) & CAN_NODE_MASK;

	return (CAN_Identifier){.tx_node_id = tx_node_id, .msg_id = msg_id, .rx_node_id = rx_node_id};
}

HAL_StatusTypeDef CubeCANExt_BuildExtendedFilter(const CAN_Identifier *const identifier, const uint32_t filter_index, const uint32_t fifo, FDCAN_FilterTypeDef *const filter)
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

HAL_StatusTypeDef CubeMXCan_AddFilter(const CubeMXCan_Handle *const handle, const FDCAN_FilterTypeDef *filter)
{
	if (handle == NULL || filter == NULL) {
		LOGOMATIC("CubeMXCan_AddFilter: Invalid handle or filter pointer\n");
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_ERROR;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL) {
			status = HAL_FDCAN_ConfigFilter(handle->hfdcan, filter);
		}
	}

	return status;
}

uint8_t CubeMXCan_Private_BytesToDlc(const uint8_t bytes)
{
	switch (bytes) {
		case 0U:
			return FDCAN_DLC_BYTES_0;
		case 1U:
			return FDCAN_DLC_BYTES_1;
		case 2U:
			return FDCAN_DLC_BYTES_2;
		case 3U:
			return FDCAN_DLC_BYTES_3;
		case 4U:
			return FDCAN_DLC_BYTES_4;
		case 5U:
			return FDCAN_DLC_BYTES_5;
		case 6U:
			return FDCAN_DLC_BYTES_6;
		case 7U:
			return FDCAN_DLC_BYTES_7;
		case 8U:
			return FDCAN_DLC_BYTES_8;
		case 12U:
			return FDCAN_DLC_BYTES_12;
		case 16U:
			return FDCAN_DLC_BYTES_16;
		case 20U:
			return FDCAN_DLC_BYTES_20;
		case 24U:
			return FDCAN_DLC_BYTES_24;
		case 32U:
			return FDCAN_DLC_BYTES_32;
		case 48U:
			return FDCAN_DLC_BYTES_48;
		case 64U:
			return FDCAN_DLC_BYTES_64;
		default:
			LOGOMATIC("CubeMXCan_Private_BytesToDlc: Invalid byte count\n");
			return FDCAN_DLC_BYTES_0;
	}
}

uint8_t CubeMXCan_Private_DlcToBytes(const uint32_t dlc)
{
	switch (dlc) {
		case FDCAN_DLC_BYTES_0:
			return 0U;
		case FDCAN_DLC_BYTES_1:
			return 1U;
		case FDCAN_DLC_BYTES_2:
			return 2U;
		case FDCAN_DLC_BYTES_3:
			return 3U;
		case FDCAN_DLC_BYTES_4:
			return 4U;
		case FDCAN_DLC_BYTES_5:
			return 5U;
		case FDCAN_DLC_BYTES_6:
			return 6U;
		case FDCAN_DLC_BYTES_7:
			return 7U;
		case FDCAN_DLC_BYTES_8:
			return 8U;
		case FDCAN_DLC_BYTES_12:
			return 12U;
		case FDCAN_DLC_BYTES_16:
			return 16U;
		case FDCAN_DLC_BYTES_20:
			return 20U;
		case FDCAN_DLC_BYTES_24:
			return 24U;
		case FDCAN_DLC_BYTES_32:
			return 32U;
		case FDCAN_DLC_BYTES_48:
			return 48U;
		case FDCAN_DLC_BYTES_64:
			return 64U;
		default:
			LOGOMATIC("CubeMXCan_Private_DlcToBytes: Invalid DLC value\n");
			return 0U;
	}
}

bool CubeMXCan_Private_IsDisabled(const CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return true;
	}

	bool disabled = true;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL) {
			const HAL_FDCAN_StateTypeDef state = HAL_FDCAN_GetState(handle->hfdcan);
			FDCAN_ProtocolStatusTypeDef protocol_status = {0};

			bool is_bus_off = false;

			if (HAL_FDCAN_GetProtocolStatus(handle->hfdcan, &protocol_status) == HAL_OK) {
				is_bus_off = protocol_status.BusOff;
			}

			disabled = (state == HAL_FDCAN_STATE_ERROR || state == HAL_FDCAN_STATE_READY || is_bus_off);
		}
	}

	return disabled;
}

HAL_StatusTypeDef CubeMXCan_Private_RecoverPeripheral(const CubeMXCan_Handle *const handle)
{
	if (handle == NULL) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status = HAL_OK;

	CRITICAL_SECTION
	{
		if (handle->hfdcan != NULL && handle->started) {
			FDCAN_ProtocolStatusTypeDef protocol_status = {0};

			if (HAL_FDCAN_GetProtocolStatus(handle->hfdcan, &protocol_status) == HAL_OK && protocol_status.BusOff) {
				LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: CRITICAL BUS-OFF DETECTED. Forcing instant hardware reset...\n");

				HAL_FDCAN_Stop(handle->hfdcan);

				status = HAL_FDCAN_Init(handle->hfdcan);

				if (status == HAL_OK) {
					status = HAL_FDCAN_ActivateNotification(handle->hfdcan, FDCAN_IT_RX_EVENTS, 0U);
				}

				if (status == HAL_OK) {
					status = HAL_FDCAN_Start(handle->hfdcan);
				}

				if (status != HAL_OK) {
					LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: Aggressive hardware start failed!\n");
				}
			}

			if (HAL_FDCAN_IsRestrictedOperationMode(handle->hfdcan)) {
				LOGOMATIC("CubeMXCan_Private_RecoverPeripheral: Forcing exit from restricted operation mode\n");
				HAL_FDCAN_ExitRestrictedOperationMode(handle->hfdcan);
			}
		}
	}

	return status;
}
