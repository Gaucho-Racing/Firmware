#include "Pinging.h"

#include "CANutils.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "StateUtils.h"

// add new pingable devices here, arrays are updated automagically
#define PING_LIST(OP)                                                                                                                                                                                  \
	OP(GRCAN_BCU, 0)                                                                                                                                                                               \
	OP(GRCAN_Dash_Panel, 1)

// Converts PING_LIST into a list of the first column (IDs)
#define PING_LIST_AS_ID(id, index) id,

// Converts PING_LIST into a lookup table from ID to index
#define PING_LIST_AS_LOOKUP(id, index) [id] = index,

const uint8_t IDsToBePinged[] = {PING_LIST(PING_LIST_AS_ID)};

const uint8_t PingsToBeIDed[] = {PING_LIST(PING_LIST_AS_LOOKUP)};

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
		ECU_CAN_Send(GRCAN_BUS_PRIMARY, IDsToBePinged[i], GRCAN_PING, &(GRCAN_PING_MSG){timestamp}, sizeof(GRCAN_PING_MSG));
	}
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
}
