#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateData.h"

#ifndef CANUTILS_H
#define CANUTILS_H

#define ECU_STATE_DATA_SEND_INTERVAL_MS 20
void ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size);
void ECU_CAN_Send_DTI(GRCAN_CUSTOM_ID msgID, void *data, uint32_t size);
void SendECUStateDataOverCAN(ECU_StateData *stateData);
void SendECUAnalogDataOverCAN(ECU_StateData *stateData);

#endif
