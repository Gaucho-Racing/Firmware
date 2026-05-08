#include "GRCAN_FancyLayer.h"

#include <stdint.h>
#include <string.h>

#include "Logomatic.h"
#include "grcan_utils.h"
#include "main.h"

static CANHandle *grcan_primary = NULL;
static CANHandle *grcan_data = NULL;
static CANHandle *grcan_data_subnet = NULL;
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

static GRCAN_NODE_ID grcan_local_node_id = GRCAN_ALL;

bool GRCAN_ValidateBusConfig(GRCAN_BusConfig *bus_config)
{
	UNUSED(bus_config);
	return true; // TODO: implement more validation checks, like valid pin numbers, valid filter counts, etc.
}

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

bool enable_port_clock(GPIO_TypeDef *port)
{
	if (port == GPIOA) {
		GPIOx_CLK_ENABLE(GPIOA);
		return true;
	} else if (port == GPIOB) {
		GPIOx_CLK_ENABLE(GPIOB);
		return true;
	} else if (port == GPIOD) {
		GPIOx_CLK_ENABLE(GPIOD);
		return true;
	} else {
		LOGOMATIC("GRCAN_InitBus: invalid GPIO port\n");
		return false;
	}
}

bool deactivate_port_clock(GPIO_TypeDef *port)
{
	if (port == GPIOA) {
		GPIOx_CLK_DISABLE(GPIOA);
		return true;
	}
	if (port == GPIOB) {
		GPIOx_CLK_DISABLE(GPIOB);
		return true;
	}
	if (port == GPIOD) {
		GPIOx_CLK_DISABLE(GPIOD);
		return true;
	} else {
		LOGOMATIC("GRCAN_DeactivateBus: invalid GPIO port\n");
		return false;
	}
}

bool GRCAN_InitBus(GRCAN_BusConfig *bus_config)
{
	CANConfig cfg = {0};
	CANHandle **slot = NULL;
	CANHandle *handle = NULL;

	if (bus_config == NULL) {
		LOGOMATIC("GRCAN_InitBus: NULL bus_config\n");
		return false;
	}

	if (!GRCAN_ValidateBusConfig(bus_config)) {
		LOGOMATIC("GRCAN_InitBus: invalid config for bus %d\n", bus_config->bus);
		return false;
	}

	enable_port_clock(bus_config->rx_pin.port);
	enable_port_clock(bus_config->tx_pin.port);

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
		LOGOMATIC("\nGRCAN_InitBus: can_init failed for bus %d\n", bus_config->bus);
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

bool GRCAN_DeactivateBus(GRCAN_BUS_ID bus)
{
	CANHandle *handle = GRCAN_GetHandle(bus);
	if (handle == NULL) {
		LOGOMATIC("GRCAN_DeactivateBus: invalid bus %d\n", bus);
		return false;
	}

	GPIO_TypeDef *rx = handle->rx_gpio;
	GPIO_TypeDef *tx = handle->tx_gpio;

	if (can_release(handle) != 0) {
		LOGOMATIC("GRCAN_DeactivateBus: can_release failed for bus %d\n", bus);
		return false;
	}

	deactivate_port_clock(rx);
	deactivate_port_clock(tx);

	switch (bus) {
		case GRCAN_BUS_PRIMARY:
			grcan_primary = NULL;
			break;
		case GRCAN_BUS_DATA:
			grcan_data = NULL;
			break;
		case GRCAN_BUS_TESTING:
			grcan_testing = NULL;
			break;
		case GRCAN_BUS_CHARGER:
			grcan_charging = NULL;
			break;
		default:
			LOGOMATIC("GRCAN_DeactivateBus: invalid bus %d\n", bus);
			return false;
	}

	return true;
}

void GRCAN_SetLocalNodeID(GRCAN_NODE_ID localID)
{
	if (localID == GRCAN_ALL) {
		LOGOMATIC("GRCAN_SetLocalNodeID: Local node ID cannot be GRCAN_ALL\n");
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

	if (id->srcID == GRCAN_ALL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Source ID cannot be GR_ALL\n");
	}
	if (id->destNode == GRCAN_ALL) {
		LOGOMATIC("GRCAN_Fancy_Decode: Destination ID cannot be GR_ALL\n");
	}
}

uint32_t GRCAN_BRS_Setting(GRCAN_BUS_ID bus)
{
	CANHandle *handle = GRCAN_GetHandle(bus);

	if (handle == NULL || handle->hal_fdcanP == NULL) {
		return FDCAN_BRS_OFF;
	}

	if (handle->hal_fdcanP->Init.FrameFormat == FDCAN_FRAME_CLASSIC || handle->hal_fdcanP->Init.FrameFormat == FDCAN_FRAME_FD_NO_BRS) {
		return FDCAN_BRS_OFF;
	}

	if (handle->hal_fdcanP->Init.FrameFormat == FDCAN_FRAME_FD_BRS) {
		return FDCAN_BRS_ON;
	}

	LOGOMATIC("GRCAN_GetBRSSetting: Invalid FDCAN configuration for bus %d\n", bus);
	return FDCAN_BRS_OFF;
}

bool GRCAN_Raw_Send(GRCAN_BUS_ID bus, uint32_t rawID, void *data, uint32_t size)
{
	CANHandle *handle = GRCAN_GetHandle(bus);
	GRCAN_BusMode mode;

	if (handle == NULL) {
		LOGOMATIC("GRCAN_Raw_Send: bus %d not configured\n", bus);
		return false;
	}

	if ((data == NULL) && (size > 0)) {
		LOGOMATIC("GRCAN_Raw_Send: NULL data with nonzero size\n");
		return false;
	}

	mode = GRCAN_BusModeForBus(bus);

	if (mode == GRCAN_MODE_CLASSIC) {
		if (size > 8) {
			LOGOMATIC("GRCAN_Raw_Send: size %lu > 8 (classic CAN)\n", (unsigned long)size);
			return false;
		}
	} else if (mode == GRCAN_MODE_FD) {
		if (size > 64) {
			LOGOMATIC("GRCAN_Raw_Send: size %lu > 64 (CAN FD)\n", (unsigned long)size);
			return false;
		}
	} else {
		LOGOMATIC("GRCAN_Raw_Send: invalid bus mode for bus %d\n", bus);
		return false;
	}

	FDCAN_TxHeaderTypeDef header = {
	    .Identifier = rawID,
	    .IdType = FDCAN_EXTENDED_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
	    .DataLength = GRCAN_to_DLC(size),
	    .BitRateSwitch = GRCAN_BRS_Setting(bus),
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
	    .MessageMarker = 0,
	};

	FDCANTxMessage msg = {0};
	msg.tx_header = header;

	if ((data != NULL) && (size > 0)) {
		memcpy(msg.data, data, size);
	}

	if (can_send(handle, &msg) != 0) {
		LOGOMATIC("GRCAN_Raw_Send: can_send failed on bus %d\n", bus);
		return false;
	}

	return true;
}

bool GRCAN_Fancy_Send(GRCAN_BUS_ID bus, GRCAN_NODE_ID destNode, GRCAN_MSG_ID messageID, void *data, uint32_t size)
{
	GRCAN_Fancy_ID id = {
	    .srcID = grcan_local_node_id,
	    .destNode = destNode,
	    .messageID = messageID,
	};

	if (id.srcID == GRCAN_ALL) {
		LOGOMATIC("GRCAN_Fancy_Send: Source ID cannot be GRCAN_ALL\n");
		return false;
	}

	return GRCAN_Raw_Send(bus, GRCAN_Fancy_EncodeID(&id), data, size);
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
