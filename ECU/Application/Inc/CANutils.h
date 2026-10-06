#include <stdint.h>
#include "CubeCAN.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateData.h"

#ifndef CANUTILS_H
#define CANUTILS_H

#define ECU_STATE_DATA_SEND_INTERVAL_MS 20
CubeCAN_Handle *ECU_GetCANHandle(GRCAN_BUS_ID bus);
HAL_StatusTypeDef ECU_CAN_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint8_t size);
HAL_StatusTypeDef ECU_CAN_Send_DTI(GRCAN_CUSTOM_ID msgID, void *data, uint8_t size);
// void SendECUStateDataOverCAN(const ECU_StateData *stateData);
// void SendECUAnalogDataOverCAN(const ECU_StateData *stateData);
// void SendECUConfigOverCAN(const ECU_StateData *stateData);
#endif
