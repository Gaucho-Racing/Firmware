#include <stdint.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "StateData.h"

#ifndef CANUTILS_H
#define CANUTILS_H

#define ECU_STATE_DATA_SEND_INTERVAL_MS 20
void ECU_CAN_Send(GR_OLD_BUS_ID bus, GR_OLD_NODE_ID destNode, GR_OLD_MSG_ID messageID, void *data, uint32_t size);
void SendECUStateDataOverCAN(ECU_StateData *stateData);

#endif
