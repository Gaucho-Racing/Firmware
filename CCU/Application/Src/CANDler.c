#include "CCUStateData.h"
#include "GR_OLD_BUS_ID.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANDler.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "Logomatic.h"
#include "bitManipulations.h"
#include "can.h"
#include "main.h"

CANHandle *primary_can = {0};

void Read_CAN(uint32_t ID, void *data, uint32_t size)
{
	uint8_t byte = ((uint8_t *)data)[5];
	GR_OLD_MSG_ID messageId = (0x000FFF00 & ID) >> 8;
	GR_OLD_NODE_ID nodeId = (0xFF00000 & ID) >> 20;

	switch (messageId) {
		case MSG_ACU_STATUS_2:
			// FIXME: if bad message do a thing
			LOGOMATIC("Received a ACU STATUS 2 msg");

			// cast *data to whatever msg dti control 10 struct there is
			// copy data from that struct into the ccu state data struct (eg GETBIT)
			state_data.ACU_S2_OVERTEMP_ERROR = GETBIT(byte, 0);
			state_data.ACU_S2_OVERVOLT_ERROR = GETBIT(byte, 1);
			state_data.ACU_S2_UNDERVOLT_ERROR = GETBIT(byte, 2);
			state_data.ACU_S2_OVERCURR_ERROR = GETBIT(byte, 3);
			state_data.ACU_S2_UNDERCURR_ERROR = GETBIT(byte, 4);
			break;

		case MSG_ACU_STATUS_3:
			LOGOMATIC("Received a ACU STATUS 3 msg");
			break;
	}
}

void CAN_Configure()
{

	CANConfig canCfg;

	// SHARED config ddata for CAN1 and CAN2
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

	canCfg.rx_callback = Read_CAN;
	canCfg.rx_interrupt_priority = 15; // TODO: Maybe make these not hardcoded
	canCfg.tx_interrupt_priority = 15;
	canCfg.tx_buffer_length = 5;

	// RX shared settings
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	// TX Shared settings
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	/*FDCAN_TxHeaderTypeDef TxHeader = {
	    .Identifier = 1,

	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
						     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	    .DataLength = 1,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
	};

	FDCANTxMessage msg = {.data = {0x80}, .tx_header = TxHeader};
	*/

	// PCLK1 from SYSCLK
	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1); // FIXME

	// CAN1 =====================================================================
	canCfg.fdcan_instance = FDCAN1;
	canCfg.rx_gpio = GPIOA;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.tx_gpio = GPIOA;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

	// RX Callback CAN1
	canCfg.rx_callback = Read_CAN; // TODO: Make sure the wrapper for this is defined correctly

	primary_can = can_init(&canCfg); // FIXME: make type *CANHANDLE, look at can.h

	// Filter 1 Definitions
	FDCAN_FilterTypeDef fdcan1_filter;

	fdcan1_filter.IdType = FDCAN_EXTENDED_ID;
	fdcan1_filter.FilterIndex = 0;
	fdcan1_filter.FilterType = FDCAN_FILTER_MASK;
	fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan1_filter.FilterID1 = LOCAL_GR_ID; // filter messages with ECU destination
	fdcan1_filter.FilterID2 = 0x00000FF;

	fdcan1_filter.FilterIndex = 1;
	fdcan1_filter.FilterID1 = 0xFF; // filter messages for all targets
	HAL_FDCAN_ConfigFilter(primary_can->hal_fdcanP, &fdcan1_filter);

	// CAN2 ======================================================

	// accept unmatched standard and extended frames into RXFIFO0 - default behaviour

	can_start(primary_can);
}
