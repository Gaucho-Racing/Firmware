#include "can_mag.h"

static GRCAN_NODE_ID destNode = GRCAN_TCM;
static GRCAN_NODE_ID localNode = GRCAN_ALL;
static GRCAN_BUS_ID busMode = GRCAN_BUS_DATA;
static GRCAN_MSG_ID msgID = CAN_MAG_DEFAULT_MSG_ID;

int can_mag_init(GRCAN_NODE_ID mag_ID)
{
	if (localNode != GRCAN_ALL) {
		LOGOMATIC("CAN MAG already initialized\n");
		return 0;
	}

	GRCAN_BusConfig busCfg = {0};
	GRCAN_SetDefaultBusConfig(&busCfg, busMode);

#if defined(STM32H5)
	busCfg.clock_source = GRCAN_CLKSRC_PLL1Q;
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
