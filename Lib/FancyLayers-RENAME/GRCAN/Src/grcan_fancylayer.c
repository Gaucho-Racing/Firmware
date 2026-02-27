#include "GRCAN_FancyLayer.h"

#include <stdint.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"

#include "Logomatic.h"
#include "can.h"
#include "main.h"

static CANHandle* grcan_primary;
static CANHandle* grcan_data;
static GR_OLD_NODE_ID grcan_local_node_id;

void GRCAN_Fancy_Init(GR_OLD_NODE_ID localID, CANHandle* primaryCAN, CANHandle* dataCAN)
{
	GRCAN_Fancy_RegisterLocalID(localID);
	GRCAN_Fancy_RegisterPrimaryCAN(primaryCAN);
	GRCAN_Fancy_RegisterDataCAN(dataCAN);
}
grcan_local_node_id = localID;

	if (primaryCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for primary CAN handle\n");
	}

	if (dataCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for data CAN handle\n");
	}

	grcan_primary = primaryCAN;
	grcan_data = dataCAN;
}

uint32_t GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id)
{
	if (id == NULL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Received NULL pointer\n");
		return 0;
	}

	return ((0xFF & id->srcID) << 20) | ((0xFFF & id->messageID) << 8) | (0xFF & id->destNode);
}

void GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id, uint32_t rawID)
{
	if (id == NULL) {
		LOGOMATIC("GRCAN_Fancy_Encode: Received NULL pointer\n");
	}

	id->srcID = (rawID >> 20) & 0xFF;
	id->destNode = rawID & 0xFF;
	id->messageID = (rawID >> 8) & 0xFFF;
}

void GRCAN_Fancy_Send(GR_OLD_BUS_ID bus, GR_OLD_NODE_ID destNode, GR_OLD_MSG_ID messageID, void *data, uint32_t size)
{
	GRCAN_Fancy_ID id = {
	    .srcID = LOCAL_GR_ID,
	    .destNode = destNode,
	    .messageID = messageID,
	};

	GRCAN_Raw_Send(bus, GRCAN_Fancy_Decode(&id), data, size);
}

void GRCAN_Raw_Send(GR_OLD_BUS_ID bus, uint32_t rawID, void *data, uint32_t size)
{
	FDCANTxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = size,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg;
	msg.tx_header = header;
	memcpy(&(msg.data), data, size);

	switch (bus) {
		case GR_OLD_BUS_PRIMARY:
			can_send(grcan_primary, &msg);
			break;
		case GR_OLD_BUS_DATA:
			can_send(grcan_data, &msg);
			break;
		default:
			LOGOMATIC("GRCAN_Raw_Send: Invalid bus ID %d\n", bus);
			break;
	}
}
