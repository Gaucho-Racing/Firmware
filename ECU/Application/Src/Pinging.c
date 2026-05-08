#include "Pinging.h"

#include <string.h>

#include "CANutils.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateUtils.h"

// add new pingable devices here, arrays are updated automagically
// MUST be in agreement with Autogen/CAN/Inc/GRCAN_MSG_DATA.h/GRCAN_ECU_PINGING_RTT_MSG
#define PING_LIST(OP)                                                                                                                                                                                  \
	OP(GRCAN_ACU, 0, GRCAN_BUS_PRIMARY)                                                                                                                                                                               \
	OP(GRCAN_GR_Inverter, 1, GRCAN_BUS_PRIMARY)                                                                                                                                                                       \
	OP(GRCAN_Fan_Controller_1, 2, GRCAN_BUS_PRIMARY)                                                                                                                                                                  \
	OP(GRCAN_Fan_Controller_2, 3, GRCAN_BUS_PRIMARY)                                                                                                                                                                  \
	OP(GRCAN_Fan_Controller_3, 4, GRCAN_BUS_PRIMARY)                                                                                                                                                                  \
	OP(GRCAN_Dash_Panel, 5, GRCAN_BUS_PRIMARY)                                                                                                                                                                        \
	OP(GRCAN_TCM, 6, GRCAN_BUS_PRIMARY)                                                                                                                                                                               \
	OP(GRCAN_TireTemp_FL, 7, GRCAN_BUS_DATA)                                                                                                                                                                       \
	OP(GRCAN_TireTemp_FR, 8, GRCAN_BUS_DATA)                                                                                                                                                                       \
	OP(GRCAN_TireTemp_RL, 9, GRCAN_BUS_DATA)                                                                                                                                                                       \
	OP(GRCAN_TireTemp_RR, 10, GRCAN_BUS_DATA)                                                                                                                                                                      \
	OP(GRCAN_Suspension_FL, 11, GRCAN_BUS_DATA)                                                                                                                                                                    \
	OP(GRCAN_Suspension_FR, 12, GRCAN_BUS_DATA)                                                                                                                                                                    \
	OP(GRCAN_Suspension_RL, 13, GRCAN_BUS_DATA)                                                                                                                                                                    \
	OP(GRCAN_Suspension_RR, 14, GRCAN_BUS_DATA)                                                                                                                                                                    \
	OP(GRCAN_InboardFloor_FL, 15, GRCAN_BUS_DATA)                                                                                                                                                                  \
	OP(GRCAN_InboardFloor_FR, 16, GRCAN_BUS_DATA)                                                                                                                                                                  \
	OP(GRCAN_InboardFloor_RL, 17, GRCAN_BUS_DATA)                                                                                                                                                                  \
	OP(GRCAN_InboardFloor_RR, 18, GRCAN_BUS_DATA)                                                                                                                                                                  \
	OP(GRCAN_BrakeTemp_FL, 19, GRCAN_BUS_DATA)                                                                                                                                                                     \
	OP(GRCAN_BrakeTemp_FR, 20, GRCAN_BUS_DATA)                                                                                                                                                                     \
	OP(GRCAN_BrakeTemp_RL, 21, GRCAN_BUS_DATA)                                                                                                                                                                     \
	OP(GRCAN_BrakeTemp_RR, 22, GRCAN_BUS_DATA)                                                                                                                                                                     \
	OP(GRCAN_DGPS, 23, GRCAN_BUS_DATA)

// Converts PING_LIST into a list of the first column (IDs)
#define PING_LIST_AS_ID(id, index, bus) id,

// Converts PING_LIST into a lookup table from ID to index
#define PING_LIST_AS_LOOKUP(id, index, bus) [id] = index,

// Converts PING_LIST into a list of buses
#define PING_LIST_AS_BUS(id, index, bus) bus,

const uint8_t IDsToBePinged[] = {PING_LIST(PING_LIST_AS_ID)};

const uint8_t PingsToBeIDed[] = {PING_LIST(PING_LIST_AS_LOOKUP)};

const GRCAN_BUS_ID BusesToBePinged[] = {PING_LIST(PING_LIST_AS_BUS)};

#define NUMBER_OF_PING_DEVICES (sizeof(IDsToBePinged) / sizeof(IDsToBePinged[0]))

static volatile uint32_t sentTimestamps[NUMBER_OF_PING_DEVICES];
static volatile uint32_t receivedTimestamps[NUMBER_OF_PING_DEVICES];
static uint8_t RTTs[NUMBER_OF_PING_DEVICES];

void pingAll(void)
{
	for (uint8_t i = 0; i < NUMBER_OF_PING_DEVICES; i++) {
		uint32_t timestamp = MillisecondsSinceBoot();

		if (receivedTimestamps[i] >= sentTimestamps[i]) {
			// no timeout
			RTTs[i] = receivedTimestamps[i] - sentTimestamps[i];
		} else {
			// timeout
			RTTs[i] = PINGTIMEOUT_VALUE;
		}

		sentTimestamps[i] = timestamp;

		// ping individually
		// ECU_CAN_Send(BusesToBePinged[i], IDsToBePinged[i], GRCAN_PING, &(GRCAN_PING_MSG){timestamp}, sizeof(GRCAN_PING_MSG));
	}
	// send data to tcm
	ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_PINGING_RTT, RTTs, sizeof(GRCAN_ECU_PINGING_RTT_MSG));

	// ping en masse
	uint32_t timestamp = MillisecondsSinceBoot();
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, 0, GRCAN_PING, &(GRCAN_PING_MSG){timestamp}, sizeof(GRCAN_PING_MSG));
	ECU_CAN_Send(GRCAN_BUS_DATA, 0, GRCAN_PING, &(GRCAN_PING_MSG){timestamp}, sizeof(GRCAN_PING_MSG));
}

uint32_t getRTT(uint8_t ID)
{
	return RTTs[PingsToBeIDed[ID]];
}

void respondToPing(uint8_t srcID, uint32_t timestamp)
{
	uint8_t index = PingsToBeIDed[srcID];
	if (timestamp == sentTimestamps[index]) {
		receivedTimestamps[index] = MillisecondsSinceBoot();
	}
	if (srcID == GRCAN_Debugger) {
		// TODO: verify if this creates a feedback-loop.
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, srcID, GRCAN_PING, &(GRCAN_PING_MSG){receivedTimestamps[index]}, sizeof(GRCAN_PING_MSG));
		ECU_CAN_Send(GRCAN_BUS_DATA, srcID, GRCAN_PING, &(GRCAN_PING_MSG){receivedTimestamps[index]}, sizeof(GRCAN_PING_MSG));
	}
}
