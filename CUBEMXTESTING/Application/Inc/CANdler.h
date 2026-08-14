#include "CubeCAN.h"

#ifndef CANDLER_H
#define CANDLER_H

void CANdler_Callback(const CubeCAN_Config_Context *const context, const CAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size);

#endif
