#include "Pinging.h"

#include "CANutils.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "StateUtils.h"

// add new pingable devices here, arrays are updated automagically
#define PING_LIST(X) \
    X(GR_BCU,        0) \
    X(GR_DASH_PANEL, 1) \
    X(GR_CCU,        2)

const uint8_t IDsToBePinged[] = {
#define AS_ID(id, idx) id,
    PING_LIST(AS_ID)};

const uint8_t PingsToBeIDed[] = {
#define AS_LOOKUP(id, idx) [id] = idx,
    PING_LIST(AS_LOOKUP)};

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
		ECU_CAN_Send(GR_OLD_BUS_PRIMARY, IDsToBePinged[i], MSG_PING, &(GR_OLD_PING_MSG){timestamp}, sizeof(GR_OLD_PING_MSG));
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
