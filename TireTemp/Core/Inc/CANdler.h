#ifndef CANDLER_H
#define CANDLER_H

#include "can.h"
#include "main.h"

extern CANHandle *can_handler;

void CANInitialize();
void CAN_sendTemp(float data[TIRETEMP_PIXELS], int msgNumber);
void CAN_callback(uint32_t ID, void *data, uint32_t size);

#endif
