#ifndef CANDLER_H
#define CANDLER_H
#include "GR_OLD_NODE_ID.h"
<<<<<<< Updated upstream
#include "can.h"
    == == ==
    =
#include "GR_OLD_MSG_ID.h"
	>>>>>>> Stashed changes

	typedef struct {
	uint16_t vehicleSpeed;
	uint8_t ECUState;
	uint8_t TSActiveButton;
	uint8_t RTDButton;
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
extern CANHandle *can_handler;
extern bool canReadyToSend;

void CANInitialize();
void CAN_sendPing(GR_OLD_NODE_ID to);
void CAN_callback(uint32_t ID, void *data, uint32_t size);
void CAN_sendECU(CANHandle *c, CAN_SEND_ECU *msg);

#endif
