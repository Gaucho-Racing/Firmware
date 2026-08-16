#include "CANdler.h"

#include "CubeCAN.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"

int value;

void CANdler_Callback(const CubeCAN_Config_Context *const context, const CAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size)
{
	value++;

	if (context == NULL || identifier == NULL || data == NULL || size == 0) {
		LOGOMATIC_ERROR("CANdler_Callback: Invalid parameters received. context: %u, identifier: %p, data: %p, size: %u\n", context->busid_user_context, (void *const)identifier,
				(void *const)data, size);
		return;
	}

	const GRCAN_BUS_ID busid = context->busid_user_context;
	const GRCAN_NODE_ID tx_node = identifier->tx_node_id;
	const GRCAN_NODE_ID rx_node = identifier->rx_node_id;
	const GRCAN_MSG_ID msg_id = identifier->msg_id;

	LOGOMATIC_VERBOSE("Received on bus %d with node %d -> %d with message %d and size %u\t%.*s\n", busid, tx_node, rx_node, msg_id, size, size, (const char *const)data);
}
