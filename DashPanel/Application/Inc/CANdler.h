#ifndef CANDLER_H
#define CANDLER_H
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "can.h"

typedef struct {
	uint8_t ECUState;
	uint8_t TSActiveButton;
	uint8_t RTDButton;
	uint8_t led_bits;

} DashStatus;

extern DashStatus dashStatus;
extern CANHandle *can_handler;
extern bool canReadyToSend;

void CANInitialize();
void CAN_sendPing(GR_OLD_NODE_ID to);
void CAN_callback(uint32_t ID, void *data, uint32_t size);
void CAN_sendECU(CANHandle *c, GR_OLD_DASH_STATUS_MSG *msg, GR_OLD_NODE_ID to);

#endif
