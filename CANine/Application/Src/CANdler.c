#include "CANdler.h"

#include <stdint.h>

#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Pinging.h"
#include "StateData.h"
#include "bitManipulations.h"

void ECU_CAN_MessageHandler(GRCAN_BUS_ID bus_id, GRCAN_MSG_ID msg_id, GRCAN_NODE_ID sender_id, uint8_t *data, uint32_t data_length)
{
	switch (msg_id) {
	}
}
