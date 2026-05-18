#include "can_inboardfloor.h"
#include "can_cfg.h"

static GRCAN_MSG_ID CAN_INBOARDFLOOR_MSG_DATA = 0x052; // TODO: Finalize with GRCAN viewer and use from Autogen instead
static GRCAN_NODE_ID sensorNode = GRCAN_ALL; // Before initialization
static GRCAN_NODE_ID localNode = LOCAL_GR_ID;
static GRCAN_NODE_ID TCMNode = GRCAN_TCM;
static GRCAN_BUS_ID mainBus = GRCAN_BUS_DATA;
static GRCAN_BUS_ID subnetBus = GRCAN_BUS_DATA_SUBNET;
static uint32_t data_length = 64;

uint32_t *forwarded_brake_temp_data = 0;
uint32_t *forwarded_TCM_data = 0;

// For messages from brake temp
void BrakeTemp_Callback(uint32_t id, void *data, uint32_t size)
{

	GRCAN_Fancy_ID GRCAN_Fancy_ID;
	GRCAN_Fancy_ID.srcID = 0;
	GRCAN_Fancy_ID.destNode = 0;
	GRCAN_Fancy_ID.messageID = 0;

	GRCAN_Fancy_DecodeID(&GRCAN_Fancy_ID, id);

	forwarded_brake_temp_data = (uint32_t *)data;

	// Forward messages to TCM via main data bus -- does not filter anything right now (should only be brake temp though)
	GRCAN_Fancy_Send(mainBus, TCMNode, GRCAN_Fancy_ID.messageID, data, size);
}

// For messages from TCM
void TCM_Callback(uint32_t id, void *data, uint32_t size)
{

	GRCAN_Fancy_ID GRCAN_Fancy_ID;
	GRCAN_Fancy_ID.srcID = 0;
	GRCAN_Fancy_ID.destNode = 0;
	GRCAN_Fancy_ID.messageID = 0;

	GRCAN_Fancy_DecodeID(&GRCAN_Fancy_ID, id);

	forwarded_TCM_data = (uint32_t *)data;
	// Forward all messages to brake temp sensor via subnet bus -- does not filter for anything right now
	GRCAN_Fancy_Send(subnetBus, sensorNode, GRCAN_Fancy_ID.messageID, data, size);

	if (GRCAN_Fancy_ID.messageID == GRCAN_PING) {
		// Send ping back to sender on main data bus
		GRCAN_Fancy_Send(mainBus, GRCAN_Fancy_ID.srcID, GRCAN_PING, data, size);
	}
}

int InboardFloor_CAN_Init(GRCAN_BUS_ID busID)
{
	int bus = (int)busID; //technically GRCAN_BUS_DATA_SUBNET does not exist, avoid warnings by casting to int

	if (bus != GRCAN_BUS_DATA && bus != GRCAN_BUS_DATA_SUBNET) {
		LOGOMATIC("Invalid bus for Inboard Floor Node CAN Init");
		return 0;
	}

	GRCAN_BusConfig bus_config = {0};
	GRCAN_SetDefaultBusConfig(&bus_config, bus);

#if defined(STM32H5)
	bus_config.clock_source = GRCAN_CLKSRC_PLL1Q; // should be 180MHz, defaults should work
#endif

	if (bus == GRCAN_BUS_DATA_SUBNET) {
		bus_config.fdcan_instance = FDCAN2;
		bus_config.rx_callback = BrakeTemp_Callback; // callback
	} else if (bus == GRCAN_BUS_DATA) {
		bus_config.fdcan_instance = FDCAN1;
		bus_config.rx_callback = TCM_Callback; // callback

        InboardFloor_Node if_node = (InboardFloor_Node)localNode;
        BrakeTemp_Node braketemp_node;

        switch(if_node) {
            case InboardFloor_FL:
                braketemp_node = BrakeTemp_FL;
                break;
            case InboardFloor_FR:
                braketemp_node = BrakeTemp_FR;
                break;
            case InboardFloor_RL:
                braketemp_node = BrakeTemp_RL;
                break;
            case InboardFloor_RR:
                braketemp_node = BrakeTemp_RR;
                break;
            default:
                LOGOMATIC("Invalid inboard floor node, defaulting to FL\n");
                braketemp_node = BrakeTemp_FL;
                break;
        }

		sensorNode = (GRCAN_NODE_ID)braketemp_node;
	}

	bool result = GRCAN_InitBus(&bus_config);

	if (!result) {
		LOGOMATIC("Failed to intialize Inboard Floor CAN Bus");
		return 0;
	}

	GRCAN_SetLocalNodeID(localNode);

	return 1; // success
}

bool InboardFloor_CAN_Send(GRCAN_NODE_ID dest_node, GRCAN_MSG_ID msg_id, void *data)
{
	if (data == NULL) {
		LOGOMATIC("Inboard Floor Node CAN send: NULL data");
		return false;
	}

	if (msg_id != CAN_INBOARDFLOOR_MSG_DATA) {
			LOGOMATIC("Invalid Inboard Floor Node message ID, defaulting to DATA\n");
			msg_id = (GRCAN_MSG_ID)CAN_INBOARDFLOOR_MSG_DATA;
	}

	bool result = GRCAN_Fancy_Send(mainBus, dest_node, msg_id, data, data_length);

	if (!result) {
		LOGOMATIC("Inboard Floor Node CAN send failed");
	}

	return result;
}
