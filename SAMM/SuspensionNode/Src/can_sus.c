#include "can_sus.h"

#include "can_cfg.h"

static GRCAN_NODE_ID destNode = GRCAN_TCM;
static GRCAN_NODE_ID localNode = GRCAN_ALL;
static GRCAN_BUS_ID busMode = GRCAN_BUS_DATA;

static CAN_MAG_MSG_ID msgID = CAN_MAG_MSG_DATA;

void SusNode_ReportBadMessageLength(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	LOGOMATIC("Bad Suspension Node CAN Rx length! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void SusNode_ReportUnhandledMessage(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id)
{
	LOGOMATIC("Unhandled Suspension Node CAN Rx msg! Bus: %d, Msg: %X, Sender: %X\n", bus_id, msg_id, sender_id);
}

void SusNode_CAN_MessageHandler(bool primary, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, void *data, uint32_t data_length)
{
	if (msg_id == GRCAN_PING) {
		// Send ping back to sender on main data bus
		GRCAN_Fancy_Send(CAN_DATAMAIN_BUS, sender_id, msg_id, data, data_length);
	}

	// Forward all messages to subnet bus
	GRCAN_Fancy_Send(CAN_SUBNET_BUS, sender_id, msg_id, data, data_length);
}

void SusNode_CAN_Send(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, GRCAN_NODE_ID dest_id, void* data, uint32_t data_length) {
	if (data_length > FDCAN_MAX_DATA_BYTES) {
		LOGOMATIC("Tried to send more than 64 bytes over suspension node CAN!");
	}

	uint32_t ID = (())
}


int can_mag_init(GRCAN_NODE_ID mag_ID, CAN_MAG_MSG_ID init_msgID)
{
	if (localNode != GRCAN_ALL) {
		LOGOMATIC("CAN MAG already initialized\n");
		return 0;
	}

	switch (init_msgID) {
		case CAN_MAG_MSG_DATA:
			msgID = init_msgID;
			break;
		case CAN_MAG_MSG_STATUS:
			msgID = init_msgID;
			break;
		case CAN_MAG_MSG_FAULT:
			msgID = init_msgID;
			break;
		default:
			LOGOMATIC("Invalid MAG message ID, defaulting to DATA\n");
			msgID = CAN_MAG_MSG_DATA;
			break;
	}

	GRCAN_BusConfig busCfg = {0};
	GRCAN_SetDefaultBusConfig(&busCfg, busMode);

#if defined(STM32H5)
	busCfg.clock_source = GRCAN_CLKSRC_PLL1Q; // should be 180MHz
						  // defaults should work
#endif

	if (mag_ID == GRCAN_SAMM_Mag_1) {
		GRCAN_SetLocalNodeID(GRCAN_SAMM_Mag_1);
		localNode = GRCAN_SAMM_Mag_1;
	} else if (mag_ID == GRCAN_SAMM_Mag_2) {
		GRCAN_SetLocalNodeID(GRCAN_SAMM_Mag_2);
		localNode = GRCAN_SAMM_Mag_2;
	} else {
		LOGOMATIC("Mag number not defined, defaulting to Mag 1\n");
		GRCAN_SetLocalNodeID(GRCAN_SAMM_Mag_1);
		localNode = GRCAN_SAMM_Mag_1;
	}

	if (!GRCAN_InitBus(&busCfg)) {
		LOGOMATIC("CAN MAG bus init failed\n");
		localNode = GRCAN_ALL;
		return 0;
	}

	return 1;
}

int can_mag_send(unsigned int *data)
{
	if (data == NULL) {
		LOGOMATIC("can_mag_send: NULL data\n");
		return 0;
	}

	if (localNode == GRCAN_ALL) {
		LOGOMATIC("can_mag_send: CAN MAG not initialized\n");
		return 0;
	}

	if (!GRCAN_Fancy_Send(busMode, destNode, msgID, (void *)data, 64)) {
		LOGOMATIC("can_mag_send failed\n");
		return 0;
	}

	return 1;
}
