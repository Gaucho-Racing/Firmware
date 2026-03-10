#include <stdint.h>

#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"

#ifndef GRCAN_FANCYLAYER_H
#define GRCAN_FANCYLAYER_H

typedef struct {
	GR_OLD_NODE_ID srcID;
	GR_OLD_NODE_ID destNode;
	GR_OLD_MSG_ID messageID;
} GRCAN_Fancy_ID;
typedef enum {
	GRCAN_MODE_CLASSIC,
	GRCAN_MODE_FD
} GRCAN_BusMode;

GRCAN_BusMode GRCAN_BusModeForBus(GR_OLD_BUS_ID bus);
void GRCAN_ConfigureBus(GR_OLD_BUS_ID bus, CANConfig *config);

uint32_t GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id);
void GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id, uint32_t rawID);

#endif
