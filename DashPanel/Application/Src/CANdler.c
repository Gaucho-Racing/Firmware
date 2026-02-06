#include "CANdler.h"

#include "can.h"
#include "stm32g4xx_hal_fdcan.h"

#define ECU_ID 1  // ID of correct ECU message - TODO: change with correct ID
#define PING_ID 2 // ID of ping message - TODO: change with correct ID

void CANInitialize()
{
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

	// FDCAN_TxHeaderTypeDef TxHeader = {
	//     .Identifier = 1,

	//     .IdType = FDCAN_STANDARD_ID,
	//     .TxFrameType = FDCAN_DATA_FRAME,
	//     .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
	// 					     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	//     .DataLength = 1,
	//     .BitRateSwitch = FDCAN_BRS_OFF,
	//     .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	//     .MessageMarker = 0			      // also change this to a real address if you change fifo control
	// };
	CANHandle *can_handler = can_init(&canCfg);
	can_start(can_handler);
}

void CAN_callback(uint32_t ID, void *data, uint32_t size)
{
	if (ID == ECU_ID) {
		// process data
	} else if (ID == PING_ID) {
		// process ping
	}
}
