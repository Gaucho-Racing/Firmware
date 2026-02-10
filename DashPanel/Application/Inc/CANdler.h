#ifndef CANDLER_H
#define CANDLER_H
#include "can.h"

typedef struct {
	uint16_t vehicleSpeed;
	uint8_t ECUState;
} DashStatus;
typedef struct {
	uint16_t vehicleSpeed;
	uint8_t ECUState;
} CAN_RECEIVE_ECU;

typedef struct {
	uint8_t TSActiveButton;
	uint8_t RTDButton;
} CAN_SEND_ECU;

extern DashStatus dashStatus;
extern CANHandle * can_handler;

void CANInitialize();
void CAN_sendPing(GR_OLD_NODE_ID to);
void CAN_callback(uint32_t ID, void *data, uint32_t size);

#endif
