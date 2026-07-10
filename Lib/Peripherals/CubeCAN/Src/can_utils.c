#include <stdint.h>

#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Private/common.h"

uint32_t Construct_CAN_Identifier(const CAN_Identifier *identifier)
{
	uint32_t message_id = 0;

	message_id |= ((0xFF & identifier->tx_node_id) << 20);
	message_id |= ((0xFFF & identifier->msg_id) << 8);
	message_id |= (0xFF & identifier->rx_node_id);

	return message_id;
}

CAN_Identifier Deconstruct_CAN_Identifier(uint32_t message_id)
{
	CAN_Identifier identifier = {0};

	identifier.tx_node_id = (GRCAN_NODE_ID)((message_id >> 20) & 0xFF);
	identifier.msg_id = (GRCAN_MSG_ID)((message_id >> 8) & 0xFFF);
	identifier.rx_node_id = (GRCAN_NODE_ID)(message_id & 0xFF);

	return identifier;
}
