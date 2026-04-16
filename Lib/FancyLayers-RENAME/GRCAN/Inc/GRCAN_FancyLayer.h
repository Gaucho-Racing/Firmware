/*
How to use:
Make sure to #define USECANx in your can_cfg.h
1:
void GRCAN_SetDefaultBusConfig(GRCAN_BusConfig *busCfg, GRCAN_BUS_ID bus);
-- applies default settings for CAN based on ECU CAN
-- default uses PB12 RD, PB13 TD, and FDCAN2
-- Make sure to change the necessary items in GRCAN_BUS_ID struct

GRCAN_BusConfig is defined in grcan_utils.h

2:
bool GRCAN_InitBus(GRCAN_BUS_ID bus)
-- initializes CAN (similar to MX_FDCANx_Init)
-- returns bool to see if succesful

3:
void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID)
-- sets local node ID for transmission
-- make sure to do this before sending

4:
bool GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
--use this to send
--returns true or false based on success

5:
bool GRCAN_DeactivateBus(GRCAN_BUS_ID bus)
--make sure to deactivate bus if you choose to send from different bus

*/

#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "can.h"
#include "can_cfg.h"
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
bool GRCAN_DeactivateBus(GRCAN_BUS_ID bus);

uint32_t GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id);
void GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id, uint32_t rawID);

bool GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size);

#endif
