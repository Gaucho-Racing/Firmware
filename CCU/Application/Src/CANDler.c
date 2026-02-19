
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CCUStateData.h"
#include "CANDler.h"
#include "can.h"
#include "GR_OLD_BUS_ID.h"
#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"
#include "GR_OLD_MSG_DAT.h"
#include "Logomatic.h"
#include "Unused.h"
#include "bitManipulations.h"
#include "main.h"

CANHandle *primary_can = {0};

void Read_CAN(uint32_t ID, void *data, uint32_t size)
{

	GR_OLD_MSG_ID messageId = (0x000FFF00 & ID) >> 8;
	GR_OLD_NODE_ID nodeId = (0xFF00000 & ID) >> 20;

	UNUSED(nodeId); // TODO Determine if calculating this value is actually needed


	switch (messageId) {
		case MSG_BCU_STATUS_2:
			// FIXME: if bad message do a thing

			if (size != sizeof(GR_OLD_BCU_STATUS_2_MSG)){
				LOGOMATIC("Bad CCU CAN Rx length! ID: %d, Size %d", ID, size);
				break;
			}

			LOGOMATIC("Received a BCU STATUS 2 msg");


			//FIXME: Might need to double check we are doing this v
			// cast *data to whatever msg dti control 10 struct there is
			// copy data from that struct into the ccu state data struct (eg GETBIT)

			/* What the rewrite would look like: STATUS 2
			*
			* GR_OLD_BCU_STATUS_2 *bcu_status_2 = (GR_OLD_BCU_STATUS_MSG_2 *)data;
			* state_data.BCU_S2_MIN_CELL_Volt = bcu_status_2->voltage_min_cell;
			* state_data.BCU_S2_MAX_CELL_TEMP = bcu_status_2->max_cell_temp;
			* state_data.BCU_S2_OVERTEMP_ERROR = GETBIT(bcu_status_2->error_bits, 0);
			* state_data.BCU_S2_OVERVOLT_ERROR = GETBIT(bcu_status_2->error_bits, 1);
			* state_data.BCU_S2_UNDERVOLT_ERROR = GETBIT(bcu_status_2->error_bits, 2);
			* state_data.BCU_S2_OVERCURR_ERROR = GETBIT(bcu_status_2->error_bits, 3);
			* state_data.BCU_S2_OVERCURR_ERROR = GETBIT(bcu_status_2->error_bits, 4);
			* state_data.BCU_S2_PRECHARGE_BITS = bcu_status_2->precharge_bits;
			*
			* break;
			*/

			uint8_t byte_3 = ((uint8_t *)data)[3];
			uint8_t byte_4 = ((uint8_t *)data)[4];
			uint8_t byte_5 = ((uint8_t *)data)[5];
			uint8_t byte_6 = ((uint8_t *)data)[6];


			// BCU_STATUS_2 MIN CELL Volt (3)
			state_data.BCU_S2_MIN_CELL_Volt = GETBITS(((uint8_t *)data)[3], 0, 8);

			/// BCU_STATUS_2 MAX CELL TEMP(4)
			state_data.BCU_S2_MAX_CELL_TEMP = GETBITS(byte_4, 0, 8);

			/// BCU_STATUS_2 Error Byte (5)
			state_data.BCU_S2_OVERTEMP_ERROR = GETBIT(byte_5, 0);
			state_data.BCU_S2_OVERVOLT_ERROR = GETBIT(byte_5, 1);
			state_data.BCU_S2_UNDERVOLT_ERROR = GETBIT(byte_5, 2);
			state_data.BCU_S2_OVERCURR_ERROR = GETBIT(byte_5, 3);
			state_data.BCU_S2_UNDERCURR_ERROR = GETBIT(byte_5, 4);

			// BCU_STATUS_2 PRECHARGE + SOFTWARE LATCH (6)
			// state_data.BCU_S2_SOFTWARE_LATCH = GETBIT(byte_6, 3);

			break;

		case MSG_BCU_STATUS_3:

			if (size != sizeof(GR_OLD_BCU_STATUS_3_MSG)){
				LOGOMATIC("Bad CCU CAN Rx length! ID: %d, Size %d", ID, size);
				break;
			}

			LOGOMATIC("Received a BCU STATUS 3 msg");

			/* What the rewrite would look like: STATUS 3
			*
			* GR_OLD_BCU_STATUS_3 *bcu_status_3 = (GR_OLD_BCU_STATUS_MSG_3 *)data;
			* state_data.BCU_S3_HV_INPUTVolt = bcu_status_3->hv_input_voltage;
			* state_data.BCU_S3_HV_OUTPUTVolt = bcu_status_3->hv_output_voltage;
			* state_data.BCU_S3_HV_INPUTCurr = bcu_status_3->hv_input_current;
			* state_data.BCU_S3_HV_OUTPUTCurr = bcu_status_3->hv_output_current;
			*
			* break;
			*/

			uint8_t byte_1 = ((uint8_t *)data)[0];
			uint8_t byte_2 = ((uint8_t *)data)[2];
			uint8_t byte_3 = ((uint8_t *)data)[4];
			uint8_t byte_4 = ((uint8_t *)data)[6];

			//BCU_STATUS_3 HV_Curr Input & Output(1-2)
			state_data.BCU_S3_HV_INPUTCurr = GETBITS(byte_1, 0, 16);
			state_data.BCU_S3_HV_OUTPUTCurr = GETBITS(byte_2, 0, 16);

			//BCU_STATUS_3 HV_Curr Input & Output(3-4)
			state_data.BCU_S3_HV_INPUTVolt = GETBITS(byte_3, 0, 16);
			state_data.BCU_S3_HV_OUTPUTVolt = GETBITS(byte_4, 0, 16);

			break;

		default:
			LOGOMATIC("Unhandled CCU CAN Rx msg! ID: %d, Size %d", ID, size);
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

void SendPrechargeStatus()
{
	FDCANTxMessage msg;
	msg.tx_header.Identifier = ((0xFF & LOCAL_GR_ID) << 20) & ((0xFFF & MSG_BCU_PRECHARGE) << 8) & (0xFF & GR_BCU);
	msg.tx_header.IdType = FDCAN_STANDARD_ID;
	msg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	msg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	msg.tx_header.DataLength = 1;
	msg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	msg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	msg.tx_header.MessageMarker = 0;

	msg.data[0] = (state_data.BCU_PRECHARGE_SET_TS_ACTIVE);

	can_send(primary_can, &msg);

	LOGOMATIC("CAN MESSAGE SENT:\n");
}
