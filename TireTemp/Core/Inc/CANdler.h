#ifndef CANDLER_H
#define CANDLER_H

#include "can.h"

extern CANHandle *can_handler;

void CANInitialize();
void CAN_sendTemp(float temp);

#endif
