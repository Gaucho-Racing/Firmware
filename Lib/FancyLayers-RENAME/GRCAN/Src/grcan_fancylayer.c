#include "GRCAN_FancyLayer.h"

#include <stdint.h>
#include <string.h>

#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "can.h"
#include "grcan_utils.h"
#include "main.h"

static CANHandle *grcan_primary;
static CANHandle *grcan_data;
static CANHandle *grcan_testing;
static CANHandle *grcan_charging;

static GR_OLD_NODE_ID grcan_local_node_id;

/*
EXAMPLE USAGE:

GRCAN_BusConfig ecu_primary_cfg = {
    .bus = GR_OLD_BUS_PRIMARY,
    .fdcan_instance = FDCAN1,

    .clock_source = GRCAN_CLKSRC_PCLK1,
    .clock_divider = GRCAN_CLK_DIV1,
    .frame_format = GRCAN_FRAME_FD_NO_BRS,
    .operating_mode = GRCAN_OPMODE_NORMAL,

    .auto_retransmission = GRCAN_FEATURE_ENABLE,
    .transmit_pause = GRCAN_FEATURE_DISABLE,
    .protocol_exception = GRCAN_FEATURE_ENABLE,

    .bit_timing = {
	.nominal = {
	    .prescaler = 1,
	    .sjw = 16,
	    .seg1 = 127,
	    .seg2 = 42
	},
	.data = {
	    .prescaler = 8,
	    .sjw = 16,
	    .seg1 = 15,
	    .seg2 = 5
	}
    },

    .std_filters_nbr = 0,
    .ext_filters_nbr = 2,

    .rx_pin = {
	.port = GPIOA,
	.pin = GPIO_PIN_11,
	.alternate_function = GPIO_AF9_FDCAN1
    },

    .tx_pin = {
	.port = GPIOA,
	.pin = GPIO_PIN_12,
	.alternate_function = GPIO_AF9_FDCAN1
    },

    .rx_callback = Read_CAN,
    .rx_interrupt_priority = 15,
    .tx_interrupt_priority = 15,
    .tx_buffer_length = 5,

    .filter_config_fn = ECU_PrimaryFilters
};

*/
void GRCAN_Abstracted_Config(GRCAN_BusConfig *bus_config)
{
	CANConfig cfg;
	CANHandle *handle;

	if (bus_config == NULL) {
		LOGOMATIC("GRCAN_Abstracted_Config: NULL bus_config\n");
		return;
	}

	can_set_clksource(GRCAN_ToHAL_ClockSource(bus_config->clock_source));

	GRCAN_FillCANConfig(&cfg, bus_config);
	GRCAN_ConfigureBus(bus_config->bus, &cfg);

	handle = GRCAN_GetHandle(bus_config->bus);
	if (handle == NULL) {
		LOGOMATIC("GRCAN_Abstracted_Config: bus %d handle is NULL after init\n", bus_config->bus);
		return;
	}

	if (bus_config->filter_config_fn != NULL) {
		bus_config->filter_config_fn(handle);
	}
}

GRCAN_BusMode GRCAN_BusModeForBus(GR_OLD_BUS_ID bus)
{
	switch (bus) {
		case GR_OLD_BUS_PRIMARY:
			return GRCAN_MODE_FD;
		case GR_OLD_BUS_TESTING:
			return GRCAN_MODE_FD;
		case GR_OLD_BUS_DATA:
			return GRCAN_MODE_CLASSIC;
		case GR_OLD_BUS_CHARGING:
			return GRCAN_MODE_CLASSIC; // update later
		default:
			LOGOMATIC("GRCAN_BusModeForBus: unknown bus %d\n", bus);
			return GRCAN_MODE_CLASSIC;
	}
}

CANHandle *GRCAN_GetHandle(GR_OLD_BUS_ID bus)
{
	switch (bus) {
		case GR_OLD_BUS_PRIMARY:
			return grcan_primary;
		case GR_OLD_BUS_DATA:
			return grcan_data;
		case GR_OLD_BUS_TESTING:
			return grcan_testing;
		case GR_OLD_BUS_CHARGING:
			return grcan_charging;
		default:
			return NULL;
	}
}

void GRCAN_ConfigureBus(GR_OLD_BUS_ID bus, CANConfig *config)
{
	CANHandle **h = NULL;
	switch (bus) {
		case GR_OLD_BUS_PRIMARY:
			h = &grcan_primary;
			break;
		case GR_OLD_BUS_DATA:
			h = &grcan_data;
			break;
		case GR_OLD_BUS_TESTING:
			h = &grcan_testing;
			break;
		case GR_OLD_BUS_CHARGING:
			h = &grcan_charging;
			break;
		default:
			LOGOMATIC("Invalid bus\n");
			return;
	}

	*h = can_init(config);
	if (*h == NULL) {
		LOGOMATIC("Failed to initialize CAN for bus %d\n", bus);
		return;
	}
	can_start(*h);
}

void GRCAN_Fancy_Init(GR_OLD_NODE_ID localID, CANHandle *primaryCAN, CANHandle *dataCAN, CANHandle *testingCAN, CANHandle *chargingCAN)
{

	grcan_local_node_id = localID;

	if (primaryCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for primary CAN handle\n");
	}

	if (dataCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for data CAN handle\n");
	}

	if (testingCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for testing CAN handle\n");
	}

	if (chargingCAN == NULL) {
		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for charging CAN handle\n");
	}

	grcan_primary = primaryCAN;
	grcan_data = dataCAN;
	grcan_testing = testingCAN;
	grcan_charging = chargingCAN;
} // different version of init function, keeping both architectures to see which works better

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
		return;
	}

	id->srcID = (rawID >> 20) & 0xFF;
	id->destNode = rawID & 0xFF;
	id->messageID = (rawID >> 8) & 0xFFF;
}

void GRCAN_Fancy_Send(GR_OLD_BUS_ID bus, GR_OLD_NODE_ID destNode, GR_OLD_MSG_ID messageID, void *data, uint32_t size)
{
	GRCAN_Fancy_ID id = {
	    .srcID = grcan_local_node_id,
	    .destNode = destNode,
	    .messageID = messageID,
	};

	if (id.srcID == GR_ALL) {
		LOGOMATIC("GRCAN_Fancy_Send: Source ID cannot be GR_ALL\n");
		return;
	}

	GRCAN_BusMode mode = GRCAN_BusModeForBus(bus);
	if (mode == GRCAN_MODE_FD) {
		GRCAN_Raw_Send_FD(bus, GRCAN_Fancy_DecodeID(&id), data, size);
		return;
	} else if (mode == GRCAN_MODE_CLASSIC) {
		GRCAN_Raw_Send_Classic(bus, GRCAN_Fancy_DecodeID(&id), data, size);
		return;
	} else {
		LOGOMATIC("GRCAN_Fancy_Send: invalid bus mode for bus %d\n", bus);
		return;
	}
}

/*
identifier can be standard (11-bit) or extended (29-bit)
TxFrameType can be data or remote frame
	data frame contains 0-8 bytes of data, RTR dominant (0) - send data
	remote frame contains no data (still specify DLC), RTR recessive (1) - request data
		RTR bit automatically encoded with FDCAN_REMOTE_FRAME and FDCAN_DATA_FRAME, no need to manually set it
ErrorStateIndicator can be active or passive
DataLength can be 0-8 bytes (classic CAN) and must be set to the number of bytes being sent
BitRateSwitch can be on or off, but we will always set it to off
TxEventFifoControl can be set to generate events on transmission
MessageMarker can be used to identify the message in the Tx event FIFO
*/

void GRCAN_Raw_Send_Classic(GR_OLD_BUS_ID bus, uint32_t rawID, void *data, uint32_t size)
{
	if (size > 8) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: size %lu > 8 (classic CAN)\n", size);
		return;
	}

	FDCANTxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_EXTENDED_ID,     // using extended ID -- src << 20 | msgID << 8 | dest
	    .TxFrameType = FDCAN_DATA_FRAME, // data frame
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = FDCAN_DLC_BYTES(size),
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	if ((data == NULL) && (size > 0)) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: NULL data with nonzero size\n");
		return;
	}
	msg.tx_header = header;
	// memcpy(&(msg.data), data, size);
	memcpy(msg.data, data, size);

	if (GRCAN_BusModeForBus(bus) != GRCAN_MODE_CLASSIC) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: bus %d is not in classic mode\n", bus);
		return;
	}

	CANHandle *handle = GRCAN_GetHandle(bus);

	if (!handle) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: bus %d not configured\n", bus);
		return;
	}

	can_send(handle, &msg);
	// switch (bus) {
	// 	case GR_OLD_BUS_PRIMARY:
	// 		if (!grcan_primary) { LOGOMATIC("ERROR: Primary CAN not configured. Primary Should use FDCAN:\n"); return; }
	// 		can_send(grcan_primary, &msg);
	// 		break;
	// 	case GR_OLD_BUS_DATA:
	// 		if (!grcan_data) { LOGOMATIC("Data CAN not configured\n"); return; }
	// 		can_send(grcan_data, &msg);
	// 		break;
	// 	case GR_OLD_BUS_TESTING:
	// 		if (!grcan_testing) { LOGOMATIC("ERROR: Testing CAN not configured. Testing Should use FDCAN:\n"); return; }
	// 		can_send(grcan_testing, &msg);
	// 		break;
	// 	case GR_OLD_BUS_CHARGING:
	// 		if (!grcan_charging) { LOGOMATIC("Charging CAN not configured\n"); return; }
	// 		can_send(grcan_charging, &msg);
	// 		break;
	// 	default:
	// 		LOGOMATIC("GRCAN_Raw_Send_Classic: Invalid bus ID %d\n", bus);
	// 		break;
	// }
}

void GRCAN_Raw_Send_FD(GR_OLD_BUS_ID bus, uint32_t rawID, void *data, uint32_t size) // FDCAN funciton allows for modification with different settings
{
	if (size > 64) {
		LOGOMATIC("GRCAN_Raw_Send_FD: size %lu > 64 (CAN FD)\n", size);
		return;
	}

	FDCANTxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_EXTENDED_ID,     // using extended ID -- src << 20 | msgID << 8 | dest
	    .TxFrameType = FDCAN_DATA_FRAME, // data frame
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = FDCAN_DLC_BYTES(size),
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	if ((data == NULL) && (size > 0)) {
		LOGOMATIC("GRCAN_Raw_Send_FD: NULL data with nonzero size\n");
		return;
	}
	msg.tx_header = header;
	// memcpy(&(msg.data), data, size);
	memcpy(msg.data, data, size);

	if (GRCAN_BusModeForBus(bus) != GRCAN_MODE_FD) {
		LOGOMATIC("GRCAN_Raw_Send_FD: bus %d is not in FD mode\n", bus);
		return;
	}

	CANHandle *handle = GRCAN_GetHandle(bus);

	if (!handle) {
		LOGOMATIC("GRCAN_Raw_Send_FD: bus %d not configured\n", bus);
		return;
	}

	can_send(handle, &msg);
	// switch (bus) {
	// 	case GR_OLD_BUS_PRIMARY:
	// 		if (!grcan_primary) { LOGOMATIC("Primary CAN not configured\n"); return; }
	// 		can_send(grcan_primary, &msg);
	// 		break;
	// 	case GR_OLD_BUS_DATA:
	// 		if (!grcan_data) { LOGOMATIC("ERROR: Data CAN not configured. Data Should use FDCAN:\n"); return; }
	// 		can_send(grcan_data, &msg);
	// 		break;
	// 	case GR_OLD_BUS_TESTING:
	// 		if (!grcan_testing) { LOGOMATIC("Testing CAN not configured\n"); return; }
	// 		can_send(grcan_testing, &msg);
	// 		break;
	// 	case GR_OLD_BUS_CHARGING:
	// 		if (!grcan_charging) { LOGOMATIC("ERROR: Charging CAN not configured. Charging Should use FDCAN:\n"); return; }
	// 		can_send(grcan_charging, &msg);
	// 		break;
	// 	default:
	// 		LOGOMATIC("GRCAN_Raw_Send_FD: Invalid bus ID %d\n", bus);
	// 		break;
	// }
}
