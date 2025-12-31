#include <stdbool.h>
#include <stdint.h>

#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"

#ifndef CANDLER_H
#define CANDLER_H

bool ECU_CAN_MessageHandler(GR_OLD_BUS_ID bus_id, GR_OLD_MSG_ID msg_id,
			    GR_OLD_NODE_ID sender_id, uint8_t *data,
			    uint8_t data_length, uint32_t timestamp);

#endif
