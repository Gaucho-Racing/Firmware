#include <stdint.h>

#include "GR_OLD_NODE_ID.h"
#include "GR_OLD_MSG_ID.h"

#ifndef GRCAN_FANCYLAYER_H
#define GRCAN_FANCYLAYER_H

typedef struct {
	GR_OLD_NODE_ID srcID;
	GR_OLD_NODE_ID destNode;
	GR_OLD_MSG_ID messageID;
} GRCAN_Fancy_ID;

uint32_t GRCAN_Fancy_Decode(GRCAN_Fancy_ID id);

void GRCAN_Fancy_Encode(GRCAN_Fancy_ID *id, uint32_t rawID);

#endif
