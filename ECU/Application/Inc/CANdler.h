#include <stdbool.h>
#include <stdint.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "StateData.h"

#ifndef CANDLER_H
#define CANDLER_H

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id, GR_OLD_NODE_ID sender_id, uint8_t *data, uint32_t data_length);

#endif
