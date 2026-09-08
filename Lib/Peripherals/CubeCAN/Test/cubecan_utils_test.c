#include <assert.h>

#include "CubeCAN.h"
#include "PrivateInc/internal.h"

int main(void)
{
	const uint8_t valid_sizes[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
	const uint8_t valid_dlcs[] = {FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2, FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8, FDCAN_DLC_BYTES_12, FDCAN_DLC_BYTES_16, FDCAN_DLC_BYTES_20, FDCAN_DLC_BYTES_24, FDCAN_DLC_BYTES_32, FDCAN_DLC_BYTES_48, FDCAN_DLC_BYTES_64};

	assert(CubeCAN_Construct_Identifier(NULL) == 0U && "Null identifiers should construct as 0");
	for (uint8_t index = 0U; index < sizeof(valid_sizes); ++index) {
		assert(CubeCAN_Private_BytesToDlc(valid_sizes[index]) == valid_dlcs[index] && "Valid byte counts should map to their dlc");
		assert(CubeCAN_Private_DlcToBytes(valid_dlcs[index]) == valid_sizes[index] && "Valid dlc values should map to their byte counts");
	}
	assert(CubeCAN_Private_BytesToDlc(9U) == FDCAN_DLC_BYTES_0 && "Unsupported byte counts should map to 0 dlc");
	assert(CubeCAN_Private_BytesToDlc(11U) == FDCAN_DLC_BYTES_0 && "Unsupported byte counts should be rejected");
	assert(CubeCAN_Private_DlcToBytes(0x10U) == 0U && "Unsupported dlc values should map to 0B");

	const CAN_Identifier identifier = {.tx_node_id = GRCAN_TireTemp_FL, .rx_node_id = GRCAN_BrakeTemp_RR, .msg_id = GRCAN_TIRE_TEMP_FRAME_7};
	const CAN_Identifier decoded = CubeCAN_Deconstruct_Identifier(CubeCAN_Construct_Identifier(&identifier));
	assert(decoded.tx_node_id == identifier.tx_node_id && "Identifier round-trip should preserve tx node");
	assert(decoded.rx_node_id == identifier.rx_node_id && "Identifier round-trip should preserve rx node");
	assert(decoded.msg_id == identifier.msg_id && "Identifier round-trip should preserve message id");

	const CAN_Identifier oversized = {.tx_node_id = (GRCAN_NODE_ID)0x1FFU, .rx_node_id = (GRCAN_NODE_ID)0x1FEU, .msg_id = (GRCAN_MSG_ID)0x1FFFU};
	const CAN_Identifier masked = CubeCAN_Deconstruct_Identifier(CubeCAN_Construct_Identifier(&oversized));
	assert(masked.tx_node_id == 0xFFU && "Tx node should be masked");
	assert(masked.rx_node_id == 0xFEU && "Rx node should be masked");
	assert(masked.msg_id == 0xFFU && "Message id should preserve its value");
	return 0;
}
