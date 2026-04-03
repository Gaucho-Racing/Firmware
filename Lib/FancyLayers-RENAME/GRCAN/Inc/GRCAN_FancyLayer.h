#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "can.h"
#include "grcan_utils.h"

#ifndef GRCAN_FANCYLAYER_H
#define GRCAN_FANCYLAYER_H

typedef struct {
	GRCAN_NODE_ID srcID;
	GRCAN_NODE_ID destNode;
	GRCAN_MSG_ID messageID;
} GRCAN_Fancy_ID;

GRCAN_BusMode GRCAN_BusModeForBus(GRCAN_BUS_ID bus);
bool GRCAN_InitBus(GRCAN_BusConfig *bus_config);

uint32_t GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id);
void GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id, uint32_t rawID);

#endif
