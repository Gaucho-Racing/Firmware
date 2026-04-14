#include <stdint.h>
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_NODE_ID.h"

#ifndef CANDLER_H
#define CANDLER_H
void ReportBadMessageLength(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id);
void ReportUnhandledMessage(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id);
void LoopbackTest(uint32_t ID, void *data, uint32_t size);
void CAN_MessageHandler(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length);


#endif
