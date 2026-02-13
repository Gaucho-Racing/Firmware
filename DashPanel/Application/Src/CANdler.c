#include "CANdler.h"

#include "can.h"
#include "dashutils.h"
#include "stm32g4xx_hal_fdcan.h"

#define ECU_ID 1  // ID of correct ECU message - TODO: change with correct ID
#define PING_ID 2 // ID of ping message - TODO: change with correct ID

CANHandle *can_handler;
DashStatus dashStatus;
bool canReadyToSend;


void CANInitialize()
{
	dashStatus = {0};
	canReadyToSend = false;

	CANConfig canCfg;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_NORMAL;
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

	// canCfg.rx_gpio = GPIOB;
	// canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// canCfg.tx_gpio = GPIOB;
	// canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	// canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// FDCAN_FilterTypeDef filter;
	// can_add_filter(can2Handle, &filter);
	/* USER CODE END 2 */

	can_handler = can_init(&canCfg);
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

void CAN_sendECU(CANHandle *c, CAN_SEND_ECU *msg)
{

	FDCANTxMessage sendECUMsg;

	// TODO: set up the message

	can_send(c, &sendECUMsg);
}

void CAN_callback(uint32_t ID, void *data, uint32_t size)
{
	// Process data
	if (ID == ECU_ID) {
		CAN_MSG_ECU *ecu_data = (CAN_MSG_ECU *)data;
		dashStatus->vehicleSpeed = ecu_data->vehicleSpeed;
		dashStatus->ECUState = ecu_data->ECUState;
		// Process data
	} else if (ID == PING_ID) {
		// process ping
		CAN_sendPing();
	}
}
