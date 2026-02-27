#include "CANdler.h"

#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "Unused.h"
#include "can.h"
#include "dashutils.h"
#include "main.h"
#include "stm32g4xx_hal_fdcan.h"

#define ECU_ID GR_ECU	 // ID of correct ECU message - TODO: change with correct ID
#define PING_ID MSG_PING // ID of ping message - TODO: change with correct ID

CANHandle *can_handler;
DashStatus dashStatus = {0};
bool canReadyToSend;

void CANInitialize()
{
	canReadyToSend = false;

	CANConfig canCfg;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK; // TODO Change
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127; // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 8;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 15; // Updated for 170MHz: (1+15+5)*8 = 168 ticks -> ~5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 5;
	canCfg.hal_fdcan_init.StdFiltersNbr = 1;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 0;

	canCfg.rx_callback = CAN_callback; // PLEASE SET
	canCfg.rx_interrupt_priority = 0;  // PLEASE SET
	canCfg.tx_interrupt_priority = 0;  // PLEASE SET
	canCfg.tx_buffer_length = 3;	   // PLEASE SET

	canCfg.fdcan_instance = FDCAN1;
	canCfg.rx_gpio = GPIOA;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.tx_gpio = GPIOA;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

	can_handler = can_init(&canCfg);
	HAL_FDCAN_ConfigGlobalFilter(can_handler->hal_fdcanP, 0, 0, 0, 0);
	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	can_start(can_handler);
}

void CAN_sendPing(GR_OLD_NODE_ID to)
{
	FDCANTxMessage pingMsg;
	pingMsg.tx_header.Identifier = (GR_DASH_PANEL << 20) | (MSG_PING << 8) | to;
	pingMsg.tx_header.IdType = FDCAN_STANDARD_ID;
	pingMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	pingMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	pingMsg.tx_header.DataLength = 4;
	pingMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	pingMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	pingMsg.tx_header.MessageMarker = 0;

	((uint32_t *)(pingMsg.data))[0] = MillisecondsSinceBoot();
	can_send(can_handler, &pingMsg);
}

void CAN_sendECU(CANHandle *c, GR_OLD_DASH_STATUS_MSG *msg, GR_OLD_NODE_ID to)
{

	FDCANTxMessage sendECUMsg;
	sendECUMsg.tx_header.Identifier = (GR_DASH_PANEL << 20) | (MSG_DASH_STATUS << 8) | to; // TODO: replace identifier with correct values
	sendECUMsg.tx_header.IdType = FDCAN_STANDARD_ID;
	sendECUMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	sendECUMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	sendECUMsg.tx_header.DataLength = sizeof(GR_OLD_DASH_STATUS_MSG);
	sendECUMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	sendECUMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	sendECUMsg.tx_header.MessageMarker = 0;

	((uint32_t *)(sendECUMsg.data))[0] = (uint32_t)msg;

	can_send(c, &sendECUMsg);
}

void CAN_callback(uint32_t ID, void *data, uint32_t size)
{
	UNUSED(size); // FIXME Validate actual size versus expected size for different messages!

	// Process data
	if (ID == MSG_ECU_STATUS_1) {
		GR_OLD_ECU_STATUS_1_MSG *ecu_data = (GR_OLD_ECU_STATUS_1_MSG *)data;
		dashStatus.ECUState = ecu_data->ecu_status;
		// Process data
	} else if (ID == MSG_DASH_CONFIG) {
		GR_OLD_DASH_CONFIG_MSG *dash_data = (GR_OLD_DASH_CONFIG_MSG *)data;
		dashStatus.led_bits = dash_data->led_bits;
	} else if (ID == PING_ID) {
		// process ping
		// TODO: fix ping
		CAN_sendPing(PING_ID);
	}
}
