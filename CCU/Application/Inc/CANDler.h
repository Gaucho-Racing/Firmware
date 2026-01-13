#include <stdint.h>
#include <stdbool.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "CCUStateData.h"

#ifndef CANDLER_H
#define CANDLER_H

void ACU_CAN_MessageHandler(CCU_StateData *state_data, GR_OLD_NODE_ID sender_id, uint8_t *data, uint32_t data_length);

#endif