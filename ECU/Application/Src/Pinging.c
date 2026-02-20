#include "Pinging.h"

#include "CANutils.h"
#include "GR_OLD_MSG_DAT.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "StateUtils.h"

const uint8_t IDsToBePinged[] = {
    GR_BCU, GR_DASH_PANEL,
    // GR_CCU,
};

const uint8_t PingsToBeIDed[] = {[GR_BCU] = 0, [GR_DASH_PANEL] = 1};

static volatile uint32_t sentTimestamps[sizeof(IDsToBePinged)];
static volatile uint32_t receivedTimestamps[sizeof(IDsToBePinged)];
static uint8_t RTTs[sizeof(IDsToBePinged)];

void pingAll(void)
{
	for (uint8_t i = 0; i < sizeof(IDsToBePinged); i++) {
		uint32_t timestamp = MillisecondsSinceBoot();

		if (receivedTimestamps[i] >= sentTimestamps[i]) {
			// no timeout
			RTTs[i] = receivedTimestamps[i] - sentTimestamps[i];
		} else {
			// timeout
			RTTs[i] = 255;
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
