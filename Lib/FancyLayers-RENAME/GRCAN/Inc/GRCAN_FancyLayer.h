#include <stdint.h>
#include "grcan_utils.h"
#include <stdbool.h>

#include "can.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "grcan_utils.h"

#ifndef GRCAN_FANCYLAYER_H
#define GRCAN_FANCYLAYER_H

typedef struct {
	GR_OLD_NODE_ID srcID;
	GR_OLD_NODE_ID destNode;
	GR_OLD_MSG_ID messageID;
} GRCAN_Fancy_ID;

GRCAN_BusMode GRCAN_BusModeForBus(GR_OLD_BUS_ID bus);
bool GRCAN_InitBus(GRCAN_BusConfig *bus_config);

uint32_t GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id);
void GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id, uint32_t rawID);

#endif
