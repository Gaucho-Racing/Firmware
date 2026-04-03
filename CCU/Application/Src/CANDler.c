
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANDler.h"

#include "CCUStateData.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Unused.h"
#include "bitManipulations.h"
#include "can.h"
#include "main.h"

CANHandle *primary_can = {0};

void Read_CAN(uint32_t ID, void *data, uint32_t size)
{

	GRCAN_MSG_ID messageId = (0x000FFF00 & ID) >> 8;

	switch (messageId) {
		case MSG_BCU_STATUS_2:
			// FIXME: if bad message do a thing

			if (size != sizeof(GRCAN_BCU_STATUS_2_MSG)) {
				LOGOMATIC("Bad CCU CAN Rx length! ID: %lu, Size %lu\n", ID, size);
				break;
			}

			LOGOMATIC("Received a BCU STATUS 2 msg\n");

			// FIXME: Might need to double check we are doing this v
			//  cast *data to whatever msg dti control 10 struct there is
			//  copy data from that struct into the ccu state data struct (eg GETBIT)

			// What the rewrite would look like: STATUS 2

			GRCAN_BCU_STATUS_2_MSG *bcu_status_2 = (GRCAN_BCU_STATUS_2_MSG *)data;
			state_data.BCU_S2_20Volt = bcu_status_2->_20v_voltage;
			state_data.BCU_S2_12Volt = bcu_status_2->_12v_voltage;
			state_data.BCU_S2_SDC_Volt = bcu_status_2->sdc_voltage;
			state_data.BCU_S2_MIN_CELL_Volt = bcu_status_2->min_cell_voltage;
			state_data.BCU_S2_MAX_CELL_TEMP = bcu_status_2->max_cell_temp;

			state_data.BCU_S2_OVERTEMP_ERROR = GETBIT(bcu_status_2->status_flags, 0);
			state_data.BCU_S2_OVERVOLT_ERROR = GETBIT(bcu_status_2->status_flags, 1);
			state_data.BCU_S2_UNDERVOLT_ERROR = GETBIT(bcu_status_2->status_flags, 2);
			state_data.BCU_S2_OVERCURR_ERROR = GETBIT(bcu_status_2->status_flags, 3);
			state_data.BCU_S2_UNDERCURR_ERROR = GETBIT(bcu_status_2->status_flags, 4);

			state_data.BCU_S2_UNDER20v_WARNING = GETBIT(bcu_status_2->status_flags, 5);
			state_data.BCU_S2_UNDER12v_WARNING = GETBIT(bcu_status_2->status_flags, 6);
			state_data.BCU_S2_UNDERVOLTSDC_WARNING = GETBIT(bcu_status_2->status_flags, 7);

			break;

		default:
			LOGOMATIC("Unhandled CCU CAN Rx msg! ID: %lu, Size %lu\n", ID, size);
			break;
	}
}

void CAN_Configure(void)
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
	// canCfg.tx_buffer_length = 5;

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

void SendPrechargeStatus(CCU_StateData *state_data)
{

	FDCANTxMessage msg;
	msg.tx_header.Identifier = ((0xFF & LOCAL_GR_ID) << 20) | ((0xFFF & MSG_BCU_PRECHARGE) << 8) | (0xFF & BCU);
	msg.tx_header.IdType = FDCAN_EXTENDED_ID;
	msg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	msg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	msg.tx_header.DataLength = FDCAN_DLC_BYTES_1;
	msg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	msg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	msg.tx_header.MessageMarker = 0;

	msg.data[0] = (state_data->BCU_PRECHARGE_SET_TS_ACTIVE);

	if (sizeof(msg) != sizeof(GRCAN_BCU_PRECHARGE_MSG)) {
		LOGOMATIC("Bad CCU CAN Tx length!, Size %u\n", sizeof(msg));
	}
	LOGOMATIC("PRECHARGE SET: %d\n", state_data->BCU_PRECHARGE_SET_TS_ACTIVE);

	can_send(primary_can, &msg);

	LOGOMATIC("CAN MESSAGE SENT:\n");
}
