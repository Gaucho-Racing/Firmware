#include <stdbool.h>
#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateData.h"

#ifndef CANDLER_H
#define CANDLER_H

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length);

void ECU_CAN_DTI_MessageHandler(ECU_StateData *state_data, GRCAN_CUSTOM_ID id, uint8_t *data, uint32_t data_length);

#endif
