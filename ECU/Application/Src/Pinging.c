#include "Pinging.h"

#include "CANutils.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "StateUtils.h"

// add node IDs of devices to be pinged here, and add their indices in here to Pings ToBe IDed
const uint8_t IDsToBePinged[] = {
    GR_BCU,
	GR_DASH_PANEL,
    //GR_CCU
};

const uint8_t PingsToBeIDed[] = {
	[GR_BCU] = 0,
	[GR_DASH_PANEL] = 1,
	//[GR_CCU] = 2
};

#define NUMBER_OF_PING_DEVICES (sizeof(IDsToBePinged)/sizeof(IDsToBePinged[0]))

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
