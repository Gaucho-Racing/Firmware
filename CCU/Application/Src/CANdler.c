
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANdler.h"

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
		case GRCAN_ACU_STATUS_1:
			if (size != sizeof(GRCAN_ACU_STATUS_1_MSG)) {
				LOGOMATIC("Bad CCU CAN Rx length! ID: %lu, Size %lu\n", ID, size);
				return;
			}
			GRCAN_ACU_STATUS_1_MSG *acu_status_1 = (GRCAN_ACU_STATUS_1_MSG *)data;
			state_data.Accumulator_Voltage = acu_status_1->accumulator_voltage;
			state_data.Accumulator_SOC = acu_status_1->accumulator_soc;
			break;

		case GRCAN_ACU_STATUS_2:
			if (size != sizeof(GRCAN_ACU_STATUS_2_MSG)) {
				LOGOMATIC("Bad CCU CAN Rx length! ID: %lu, Size %lu\n", ID, size);
				return;
			}


			GRCAN_ACU_STATUS_2_MSG *acu_status_2 = (GRCAN_ACU_STATUS_2_MSG *)data;

			state_data.ACU_S2_OVERTEMP_ERROR = GETBIT(acu_status_2->status_flags, 0);
			state_data.ACU_S2_OVERVOLT_ERROR = GETBIT(acu_status_2->status_flags, 1);
			state_data.ACU_S2_UNDERVOLT_ERROR = GETBIT(acu_status_2->status_flags, 2);
			state_data.ACU_S2_OVERCURR_ERROR = GETBIT(acu_status_2->status_flags, 3);
			state_data.ACU_S2_UNDERCURR_ERROR = GETBIT(acu_status_2->status_flags, 4);

			state_data.ACU_S2_UNDER20v_WARNING = GETBIT(acu_status_2->status_flags, 5);
			state_data.ACU_S2_UNDER12v_WARNING = GETBIT(acu_status_2->status_flags, 6);
			state_data.ACU_S2_UNDERVOLTSDC_WARNING = GETBIT(acu_status_2->status_flags, 7);

			state_data.ACU_S2_IR_MINUS = GETBIT(acu_status_2->precharge_latch_flags, 1);
			state_data.ACU_S2_IR_PLUS = GETBIT(acu_status_2->precharge_latch_flags, 2);

			state_data.Max_Cell_Temp = acu_status_2->max_cell_temp;
			break;
		case GRCAN_ACU_STATUS_3:
			// Currently unused
			break;
		default:
			// Unhandled message ID, ignoring
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
	canCfg.hal_fdcan_init.StdFiltersNbr = 0;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 2;

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

	FDCAN_FilterTypeDef fdcan1_filter_ccu = {0};
	fdcan1_filter_ccu.IdType = FDCAN_EXTENDED_ID;
	fdcan1_filter_ccu.FilterIndex = 0;
	fdcan1_filter_ccu.FilterType = FDCAN_FILTER_MASK;
	fdcan1_filter_ccu.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan1_filter_ccu.FilterID1 = GRCAN_CCU & 0xFF;
	fdcan1_filter_ccu.FilterID2 = 0x000000FF;

	FDCAN_FilterTypeDef fdcan1_filter_all = {0};
	fdcan1_filter_all.IdType = FDCAN_EXTENDED_ID;
	fdcan1_filter_all.FilterIndex = 1;
	fdcan1_filter_all.FilterType = FDCAN_FILTER_MASK;
	fdcan1_filter_all.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan1_filter_all.FilterID1 = GRCAN_ALL & 0xFF;
	fdcan1_filter_all.FilterID2 = 0x000000FF;

	can_add_filter(primary_can, &fdcan1_filter_ccu);
	can_add_filter(primary_can, &fdcan1_filter_all);

	can_start(primary_can);
}


void SendPrechargeStatus(bool setPrecharge)
{
	FDCANTxMessage msg;
	msg.tx_header.Identifier = ((0xFF & GRCAN_CCU) << 20) | ((0xFFF & GRCAN_ACU_PRECHARGE) << 8) | (0xFF & GRCAN_ACU);
	msg.tx_header.IdType = FDCAN_EXTENDED_ID;
	msg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
	msg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	msg.tx_header.DataLength = FDCAN_DLC_BYTES_1;
	msg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
	msg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	msg.tx_header.MessageMarker = 0;

	msg.data[0] = setPrecharge;

	LOGOMATIC("PRECHARGE SET: %d\n", setPrecharge);

	can_send(primary_can, &msg);

	LOGOMATIC("CAN MESSAGE SENT:\n");
}
