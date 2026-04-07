#include "GRCAN_FancyLayer.h"

#include <stdint.h>
#include <string.h>

#include "Logomatic.h"
#include "grcan_utils.h"
#include "main.h"
// #include "stm32g4xx_hal_fdcan.h"
// #include "stm32g4xx_hal_gpio.h"

static CANHandle *grcan_primary = NULL;
static CANHandle *grcan_data = NULL;
static CANHandle *grcan_testing = NULL;
static CANHandle *grcan_charging = NULL;

// typedef struct {
// 	FDCAN_HandleTypeDef *hal_fdcanP;
// 	CircularBuffer *tx_buffer;
// 	uint32_t tx_buffer_length;

// can change rx callback settings to custom callback, check message size and count errors
// 	CAN_RXCallback rx_callback;

// 	// for release
// 	GPIO_TypeDef *rx_gpio;
// 	GPIO_TypeDef *tx_gpio;
// 	uint32_t Clock_Source;

// 	// state
// 	bool init;
// 	bool started;

// 	// error states
// } CANHandle;

static GRCAN_NODE_ID grcan_local_node_id = ALL;

bool GRCAN_ValidateBusConfig(GRCAN_BusConfig *bus_config) {
	return true; // TODO: implement more validation checks, like valid pin numbers, valid filter counts, etc.
}

void GRCAN_Raw_Send_Classic(GRCAN_BUS_ID bus, uint32_t rawID, void *data, uint32_t size);
void GRCAN_Raw_Send_FD(GRCAN_BUS_ID bus, uint32_t rawID, void *data, uint32_t size);

// typedef struct {
// 	FDCAN_HandleTypeDef *hal_fdcanP;
// 	CircularBuffer *tx_buffer;
// 	uint32_t tx_buffer_length;

// 	CAN_RXCallback rx_callback;

// 	// for release
// 	GPIO_TypeDef *rx_gpio;
// 	GPIO_TypeDef *tx_gpio;
// 	uint32_t Clock_Source;

// 	// state
// 	bool init;
// 	bool started;

// 	// error states
// } CANHandle;

// static GR_OLD_NODE_ID grcan_local_node_id;

// can change rx callback settings to custom callback, check message size and count errors

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

CANHandle *GRCAN_GetHandle(GRCAN_BUS_ID bus)
{
	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			return grcan_primary;
		case GRCAN_BUS_DATA:
			return grcan_data;
		case GRCAN_BUS_TESTING:
			return grcan_testing;
		case GRCAN_BUS_CHARGER:
			return grcan_charging;
		default:
			return NULL;
	}
}

// typedef struct {
// 	// can baud rate is set by fdcan prescaler and RCC clock configurations
// 	FDCAN_GlobalTypeDef *fdcan_instance; // Base address of FDCAN peripheral in memory (FDCAN1, FDCAN2, FDCAN3 macros)

// 	FDCAN_InitTypeDef hal_fdcan_init;
// 	CAN_RXCallback rx_callback;
// 	uint32_t rx_interrupt_priority;
// 	uint32_t tx_interrupt_priority;

// 	// Circular Buffer
// 	uint32_t tx_buffer_length;

// 	GPIO_TypeDef *rx_gpio;	       // Instance name, like GPIOA, GPIOB, etc.
// 	GPIO_InitTypeDef init_rx_gpio; // GPIO Parameters - set correct Alternate Function, no pullup/pulldown, high/very_high frequency
// 	GPIO_TypeDef *tx_gpio;
// 	GPIO_InitTypeDef init_tx_gpio;

// 	// additional parameters
// } CANConfig;

bool GRCAN_InitBus(GRCAN_BusConfig *bus_config)
{
	CANConfig cfg = {0};
	CANHandle **slot = NULL;
	CANHandle *handle = NULL;

	if (bus_config == NULL) {
		LOGOMATIC("GRCAN_InitBus: NULL bus_config\n");
		return false;
	}

	if (bus_config == NULL) {
		GRCAN_SetDefaultBusConfig(bus_config, bus_config->bus);
	}

	if (!GRCAN_ValidateBusConfig(bus_config)) {
		LOGOMATIC("GRCAN_InitBus: invalid config for bus %d\n", bus_config->bus);
		return false;
	}

	can_set_clksource(GRCAN_ToHAL_ClockSource(bus_config->clock_source));

	cfg.fdcan_instance = bus_config->fdcan_instance;

	cfg.hal_fdcan_init.ClockDivider = GRCAN_ToHAL_ClockDivider(bus_config->clock_divider);
	cfg.hal_fdcan_init.FrameFormat = GRCAN_ToHAL_FrameFormat(bus_config->frame_format);
	cfg.hal_fdcan_init.Mode = GRCAN_ToHAL_OperatingMode(bus_config->operating_mode);

	cfg.hal_fdcan_init.AutoRetransmission = GRCAN_ToHAL_FeatureState(bus_config->auto_retransmission);
	cfg.hal_fdcan_init.TransmitPause = GRCAN_ToHAL_FeatureState(bus_config->transmit_pause);
	cfg.hal_fdcan_init.ProtocolException = GRCAN_ToHAL_FeatureState(bus_config->protocol_exception);

	cfg.hal_fdcan_init.NominalPrescaler = bus_config->bit_timing.nominal.prescaler;
	cfg.hal_fdcan_init.NominalSyncJumpWidth = bus_config->bit_timing.nominal.sjw;
	cfg.hal_fdcan_init.NominalTimeSeg1 = bus_config->bit_timing.nominal.seg1;
	cfg.hal_fdcan_init.NominalTimeSeg2 = bus_config->bit_timing.nominal.seg2;

	cfg.hal_fdcan_init.DataPrescaler = bus_config->bit_timing.data.prescaler;
	cfg.hal_fdcan_init.DataSyncJumpWidth = bus_config->bit_timing.data.sjw;
	cfg.hal_fdcan_init.DataTimeSeg1 = bus_config->bit_timing.data.seg1;
	cfg.hal_fdcan_init.DataTimeSeg2 = bus_config->bit_timing.data.seg2;

	cfg.hal_fdcan_init.StdFiltersNbr = bus_config->std_filters_nbr;
	cfg.hal_fdcan_init.ExtFiltersNbr = bus_config->ext_filters_nbr;

	cfg.rx_callback = bus_config->rx_callback;
	cfg.rx_interrupt_priority = bus_config->rx_interrupt_priority;
	cfg.tx_interrupt_priority = bus_config->tx_interrupt_priority;
	// cfg.tx_buffer_length = bus_config->tx_buffer_length;

	cfg.rx_gpio = bus_config->rx_pin.port;
	cfg.init_rx_gpio.Pin = bus_config->rx_pin.pin;
	cfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	cfg.init_rx_gpio.Pull = GPIO_NOPULL;
	cfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	cfg.init_rx_gpio.Alternate = bus_config->rx_pin.alternate_function;

	cfg.tx_gpio = bus_config->tx_pin.port;
	cfg.init_tx_gpio.Pin = bus_config->tx_pin.pin;
	cfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	cfg.init_tx_gpio.Pull = GPIO_NOPULL;
	cfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	cfg.init_tx_gpio.Alternate = bus_config->tx_pin.alternate_function;

	switch (bus_config->bus) {
		case GRCAN_BUS_PRIMARY:
			slot = &grcan_primary;
			break;
		case GRCAN_BUS_DATA:
			slot = &grcan_data;
			break;
		case GRCAN_BUS_TESTING:
			slot = &grcan_testing;
			break;
		case GRCAN_BUS_CHARGER:
			slot = &grcan_charging;
			break;
		default:
			LOGOMATIC("GRCAN_InitBus: invalid bus %d\n", bus_config->bus);
			return false;
	}

	handle = can_init(&cfg);
	if (handle == NULL) {
		LOGOMATIC("GRCAN_InitBus: can_init failed for bus %d\n", bus_config->bus);
		return false;
	}

	*slot = handle;

	if (bus_config->filter_config != NULL) {
		FDCAN_FilterTypeDef filter = {
		    .IdType = bus_config->filter_config->id_type,
		    .FilterIndex = bus_config->filter_config->filter_index,
		    .FilterType = bus_config->filter_config->filter_type,
		    .FilterConfig = bus_config->filter_config->filter_config,
		    .FilterID1 = bus_config->filter_config->filter_id1,
		    .FilterID2 = bus_config->filter_config->filter_id2,
		};

		if (HAL_FDCAN_ConfigFilter(handle->hal_fdcanP, &filter) != HAL_OK) {
			LOGOMATIC("GRCAN_InitBus: filter config failed for bus %d\n", bus_config->bus);
			return false;
		}
	}

	if (can_start(handle) != 0) {
		LOGOMATIC("GRCAN_InitBus: can_start failed for bus %d\n", bus_config->bus);
		return false;
	}

	return true;
}

// void GRCAN_Fancy_Init(GR_OLD_NODE_ID localID, CANHandle *primaryCAN, CANHandle *dataCAN, CANHandle *testingCAN, CANHandle *chargingCAN)
// {

// 	grcan_local_node_id = localID;

// 	if (primaryCAN == NULL) {
// 		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for primary CAN handle\n");
// 	}

// 	if (dataCAN == NULL) {
// 		LOGOMATIC("GRCAN_Fancy_Ini
// 			t: Received NULL pointer for data CAN handle\n");
// 	}

// 	if (testingCAN == NULL) {
// 		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for testing CAN handle\n");
// 	}

// 	if (chargingCAN == NULL) {
// 		LOGOMATIC("GRCAN_Fancy_Init: Received NULL pointer for charging CAN handle\n");
// 	}

// 	grcan_primary = primaryCAN;
// 	grcan_data = dataCAN;
// 	grcan_testing = testingCAN;
// 	grcan_charging = chargingCAN;
// }

void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID)
{
	if (localID == ALL) {
		LOGOMATIC("GRCAN_SetLocalNodeID: Local node ID cannot be GR_ALL\n");
		return;
	}

	grcan_local_node_id = localID;
}

uint32_t GRCAN_Fancy_EncodeID(GRCAN_Fancy_ID *id)
{
	if (id == NULL) {
		LOGOMATIC("GRCAN_Fancy_Encode: Received NULL pointer\n");
		return 0;
	}

	return ((0xFF & id->srcID) << 20) | ((0xFFF & id->messageID) << 8) | (0xFF & id->destNode);
}

void GRCAN_Fancy_DecodeID(GRCAN_Fancy_ID *id, uint32_t rawID)
{
	if (id == NULL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Received NULL pointer\n");
		return;
	}

	if ((rawID & ~0x0FFFFFFFU) != 0U) {
		LOGOMATIC("GRCAN_Fancy_Decode: ID field sizes are too large\n");
		return;
	}

	id->srcID = (rawID >> 20) & 0xFF;
	id->destNode = rawID & 0xFF;
	id->messageID = (rawID >> 8) & 0xFFF;

	if (id->srcID == ALL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Source ID cannot be GR_ALL\n");
	}

	if (id->destNode == ALL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Destination ID cannot be GR_ALL\n");
	}
}

void GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
{
	GRCAN_Fancy_ID id = {
	    .srcID = grcan_local_node_id,
	    .destNode = destNode,
	    .messageID = messageID,
	};

	if (id.srcID == ALL) {
		LOGOMATIC("GRCAN_Fancy_Send: Source ID cannot be GR_ALL\n");
		return;
	}

	GRCAN_BusMode mode = GRCAN_BusModeForBus(bus);
	if (mode == GRCAN_MODE_FD) {
		GRCAN_Raw_Send_FD(bus, GRCAN_Fancy_EncodeID(&id), data, size);
		return;
	} else if (mode == GRCAN_MODE_CLASSIC) {
		GRCAN_Raw_Send_Classic(bus, GRCAN_Fancy_EncodeID(&id), data, size);
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

void GRCAN_Raw_Send_Classic(GRCAN_BUS_ID bus, uint32_t rawID, void *data, uint32_t size)
{
	if (size > 8) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: size %lu > 8 (classic CAN)\n", size);
		return;
	}

	if ((data == NULL) && (size > 0)) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: NULL data with nonzero size\n");
		return;
	}

	if (GRCAN_BusModeForBus(bus) != GRCAN_MODE_CLASSIC) {
		LOGOMATIC("GRCAN_Raw_Send_Classic: bus %d is not in classic mode\n", bus);
		return;
	}

	FDCAN_TxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_EXTENDED_ID,     // using extended ID -- src << 20 | msgID << 8 | dest
	    .TxFrameType = FDCAN_DATA_FRAME, // data frame
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = GRCAN_to_DLC(size),
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	msg.tx_header = header;
	// memcpy(&(msg.data), data, size);
	memcpy(msg.data, data, size);

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

void GRCAN_Raw_Send_FD(GRCAN_BUS_ID bus, uint32_t rawID, void *data, uint32_t size) // FDCAN funciton allows for modification with different settings
{
	if (size > 64) {
		LOGOMATIC("GRCAN_Raw_Send_FD: size %lu > 64 (CAN FD)\n", size);
		return;
	}

	if ((data == NULL) && (size > 0)) {
		LOGOMATIC("GRCAN_Raw_Send_FD: NULL data with nonzero size\n");
		return;
	}

	if (GRCAN_BusModeForBus(bus) != GRCAN_MODE_FD) {
		LOGOMATIC("GRCAN_Raw_Send_FD: bus %d is not in FD mode\n", bus);
		return;
	}

	FDCAN_TxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_EXTENDED_ID,     // using extended ID -- src << 20 | msgID << 8 | dest
	    .TxFrameType = FDCAN_DATA_FRAME, // data frame
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = GRCAN_to_DLC(size),
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	msg.tx_header = header;
	// memcpy(&(msg.data), data, size);
	memcpy(msg.data, data, size);

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
