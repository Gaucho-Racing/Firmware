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

uint8_t CubeMXCan_Private_DlcToBytes(const uint32_t dlc)
{
	uint32_t dlc_index = (dlc >> 16U) & 0x0FU;

	if (dlc <= 8U) {
		return (uint8_t)dlc;
	}

	if (dlc_index == 0U) {
		dlc_index = dlc & 0x0FU;
	}

	static const uint8_t dlc_to_bytes[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

	return (dlc_index > 15U) ? 0U : dlc_to_bytes[dlc_index];
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
					status = HAL_FDCAN_ActivateNotification(handle->hfdcan, rx_events, 0U);
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
