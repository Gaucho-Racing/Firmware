#include "can_sus.h"

#include "can_cfg.h"

// static GRCAN_NODE_ID localNode = GRCAN_ALL;
static GRCAN_NODE_ID sensorNode = GRCAN_ALL; // q from shravya: what is GRCAN_ALL?
static GRCAN_NODE_ID localNode = LOCAL_GR_ID;
static GRCAN_NODE_ID TCMNode = GRCAN_TCM;
static GRCAN_BUS_ID busMode = GRCAN_BUS_DATA;
static GRCAN_NODE_ID sensorNode; // tire temp -> q from shravya: sensorNode declared twice?
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

	// q from shravya: doesn't this send from TCM to tire temp instead of other way around?
	if (GRCAN_Fancy_ID.srcID == TCMNode) {
		GRCAN_Fancy_Send(busMode, sensorNode, GRCAN_Fancy_ID.messageID, data, size);
	}
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
	// q from shravya: busMode is just the default bus... need to indicate that it's being sent thru
	// subnet bus somehow?
	GRCAN_Fancy_Send(busMode, sensorNode, GRCAN_Fancy_ID.messageID, data, size);

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

	GRCAN_BusConfig bus_config;
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

int SusNode_CAN_Send(void *data) {
	// send IMU and MAG data to TCM
	typedef struct {
		
	}
}

/* ================================================================================================== */

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
