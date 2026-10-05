#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateData.h"

#ifndef CANDLER_H
#define CANDLER_H

#define DTI_RX_ID 0x00000016u
#define DTI_RX_MASK 0x1FFFC0FFu
#define CANDLER_IS_DTI_ID(raw_id) (((raw_id) & DTI_RX_MASK) == DTI_RX_ID)

void CANdler_Callback(const CubeCAN_Config_Context *const context, const CubeCAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size);

void ECU_CAN_MessageHandler(ECU_StateData *state_data, GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, const uint8_t *data, uint8_t data_length);

void ECU_CAN_DTI_MessageHandler(ECU_StateData *state_data, uint32_t id, const uint8_t *data, uint8_t data_length);

#endif
