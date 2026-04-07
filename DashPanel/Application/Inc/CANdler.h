#ifndef CANDLER_H
#define CANDLER_H
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "can.h"

typedef struct {
	uint8_t ECUState;
	uint8_t button_flags;
	uint8_t led_bits;
	uint8_t button1;
	uint8_t button2;
	uint8_t button3;
	uint8_t button4;
} DashStatus;

extern DashStatus dashStatus;
extern CANHandle *can_handler;
extern bool canReadyToSend;

void CANInitialize();
void CAN_sendPing(GRCAN_NODE_ID to, uint32_t data);
void CAN_callback(uint32_t ID, void *data, uint32_t size);
void CAN_sendECU(CANHandle *c, GRCAN_DASH_STATUS_MSG *msg, GRCAN_NODE_ID to);

#endif
