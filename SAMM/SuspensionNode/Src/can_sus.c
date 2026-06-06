#include "can_sus.h"

#include "can_cfg.h"

static GRCAN_MSG_ID CAN_SUS_MSG_DATA = GRCAN_SUSPENSION_IMU_MAG_DATA;
static GRCAN_NODE_ID localNode = LOCAL_GR_ID;
static GRCAN_BUS_ID mainBus = GRCAN_BUS_DATA;
static GRCAN_BUS_ID subnetBus = GRCAN_BUS_DATA_SUBNET;
static uint32_t data_length = 64;

uint32_t *forwarded_tire_temp_data = 0;
uint32_t *forwarded_TCM_data = 0;

// For messages from tire temp
void TireTemp_Callback(uint32_t id, void *data, uint32_t size)
{
	GRCAN_Raw_Send(mainBus, id, data, size);
}

// For messages from TCM
void TCM_Callback(uint32_t id, void *data, uint32_t size)
{
	GRCAN_Raw_Send(subnetBus, id, data, size);

	GRCAN_Fancy_ID GRCAN_Fancy_ID = {0};
	GRCAN_Fancy_DecodeID(&GRCAN_Fancy_ID, id);
	if (GRCAN_Fancy_ID.messageID == GRCAN_PING) {
		GRCAN_Fancy_Send(mainBus, GRCAN_Fancy_ID.srcID, GRCAN_PING, data, size);
	}
}

int SusNode_CAN_Init(GRCAN_BUS_ID busID)
{
	int bus = (int)busID; // technically GRCAN_BUS_DATA_SUBNET does not exist, avoid warnings by casting to int

	if (bus != GRCAN_BUS_DATA && bus != GRCAN_BUS_DATA_SUBNET) {
		LOGOMATIC("Invalid bus for Suspension Node CAN Init");
		return 0;
	}

	GRCAN_BusConfig bus_config = {0};
	GRCAN_SetDefaultBusConfig(&bus_config, bus);

#if defined(STM32H5)
	bus_config.clock_source = GRCAN_CLKSRC_PLL1Q; // should be 180MHz, defaults should work
#endif

	if (bus == GRCAN_BUS_DATA_SUBNET) {
		bus_config.fdcan_instance = FDCAN2;
		bus_config.rx_callback = TireTemp_Callback; // callback
	} else if (bus == GRCAN_BUS_DATA) {
		bus_config.fdcan_instance = FDCAN1;
		bus_config.rx_callback = TCM_Callback; // callback
	}

	// TODO Comment in/out for testing loopback
	// bus_config.operating_mode = GRCAN_OPMODE_EXTERNAL_LOOPBACK;

	bool result = GRCAN_InitBus(&bus_config);

	if (!result) {
		LOGOMATIC("Failed to intialize Suspension CAN Bus");
		return 0;
	}

	GRCAN_SetLocalNodeID(localNode);

	return 1; // success
}

bool SusNode_CAN_Send(GRCAN_NODE_ID dest_node, GRCAN_MSG_ID msg_id, void *data)
{
	if (data == NULL) {
		LOGOMATIC("Suspension Node CAN send: NULL data");
		return false;
	}

	if (msg_id != CAN_SUS_MSG_DATA) {
		LOGOMATIC("Invalid Suspension Node message ID, defaulting to DATA\n");
		msg_id = (GRCAN_MSG_ID)CAN_SUS_MSG_DATA;
	}

	bool result = GRCAN_Fancy_Send(mainBus, dest_node, msg_id, data, data_length);

	if (!result) {
		LOGOMATIC("Suspension Node CAN send failed");
	}

	return result;
}
