#include "can_sus.h"

#include "can_cfg.h"

// static GRCAN_NODE_ID localNode = GRCAN_ALL;
static GRCAN_NODE_ID sensorNode = GRCAN_ALL; // q from shravya: what is GRCAN_ALL?
static GRCAN_NODE_ID localNode = LOCAL_GR_ID;
static GRCAN_NODE_ID TCMNode = GRCAN_TCM;
static GRCAN_BUS_ID busMode = GRCAN_BUS_DATA;
static GRCAN_BUS_ID subnetBusMode = GRCAN_BUS_DATA_SUBNET; // still needs to be defined
static GRCAN_NODE_ID sensorNode;			   // tire temp -> q from shravya: sensorNode declared twice?
static data_length = 64;

// get rid of mag functions once their functionality added
// fix callbacks for all requried functionality
// might need to implement logic for receiving and forwarding for specific sensor location (FL -> FL)
// might be better way to do this
// check if came from tire temp or tcm in the callback-- should not interfere
// might need to get rid of TCM checking if you really want to forward everything
// handle sensorNode logic in init, you can use enum in can_sus.h

// For messages from tire temp
void TireTemp_Callback(uint32_t id, void *data, uint32_t size)
{

	GRCAN_Fancy_ID GRCAN_Fancy_ID;
	GRCAN_Fancy_ID.srcID = 0;
	GRCAN_Fancy_ID.destNode = 0;
	GRCAN_Fancy_ID.messageID = 0;

	GRCAN_Fancy_DecodeID(&GRCAN_Fancy_ID, id);

	uint32_t forward_data = (uint32_t *)data;

	GRCAN_Fancy_Send(busMode, TCMNode, GRCAN_Fancy_ID.messageID, data, size);
}

// For messages from TCM
void TCM_Callback(uint32_t id, void *data, uint32_t size)
{

	GRCAN_Fancy_ID GRCAN_Fancy_ID;
	GRCAN_Fancy_ID.srcID = 0;
	GRCAN_Fancy_ID.destNode = 0;
	GRCAN_Fancy_ID.messageID = 0;

	GRCAN_Fancy_DecodeID(&GRCAN_Fancy_ID, id);

	// Forward all messages to subnet bus
	GRCAN_Fancy_Send(subnetBusMode, sensorNode, GRCAN_Fancy_ID.messageID, data, size);

	if (GRCAN_Fancy_ID.messageID == GRCAN_PING) {
		// Send ping back to sender on main data bus
		GRCAN_Fancy_Send(busMode, GRCAN_Fancy_ID.srcID, GRCAN_Fancy_ID.messageID, data, size);
	}
}

int SusNode_CAN_Init(CAN_SAMM_ROUTING_BUS bus)
{
	if (sensorNode != GRCAN_ALL) {
		LOGOMATIC("Suspension Node already initialized");
		return 0;
	}

	GRCAN_BusConfig bus_config = {0};
	GRCAN_SetDefaultBusConfig(&bus_config, bus);

#if defined(STM32H5)
	bus_config.clock_source = GRCAN_CLKSRC_PLL1Q; // should be 180MHz, defaults should work
#endif

	if (bus == CAN_SUBNET_BUS) {
		bus_config.fdcan_instance = FDCAN2;
		bus_config.rx_callback = TireTemp_Callback; // callback
	} else if (bus == CAN_DATAMAIN_BUS) {
		bus_config.fdcan_instance = FDCAN1;
		bus_config.rx_callback = TCM_Callback; // callback
	}

	bool result = GRCAN_InitBus(&bus_config);

	if (!result) {
		LOGOMATIC("Failed to intialize Suspension CAN Bus");
		return 0;
	}

	GRCAN_SetLocalNodeID(LOCAL_GR_ID);
	return 1; // success
}

bool SusNode_CAN_Send(GRCAN_NODE_ID dest_node, GRCAN_MSG_ID msg_id, void *data)
{
	if (data == NULL) {
		LOGOMATIC("Suspension Node CAN send: NULL data");
	}

	if (localNode == GRCAN_ALL) {
		LOGOMATIC("Suspension Node not initialized");
	}

	bool result = GRCAN_Fancy_Send(busMode, dest_node, msg_id, data, data_length);

	if (!result) {
		LOGOMATIC("Suspension Node CAN send failed");
		return 0;
	}

	return 1;
}

/* ================================================================================================== */

/*
todo: error checker stuff

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

*/
