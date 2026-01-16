#include "can_tests.h"
#include "can.h"
#include <string.h>

//each family has a constant number of CAN peripherals


int can_test_instance(FDCAN_HandleTypeDef) {

    return 0;
}

void can_test_rx_callback2(void*data, uint32_t size) {
	LOGOMATIC("CAN2 Got data!\n");
	// Is within an ISR, so needs to exit quickly
	return;
}

void can_test_rx_callback1(void*data, uint32_t size) {
	LOGOMATIC("CAN1 Got data!\n");

	// Is within an ISR, so needs to exit quickly
	return;
}

int can_test(void) {

    CANConfig canCfg;
	//canCfg.fdcan_instance = FDCAN2;

 	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_NORMAL;
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127;  // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 8;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 15;      // Updated for 170MHz: (1+15+5)*8 = 168 ticks -> ~5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 5;
	canCfg.hal_fdcan_init.StdFiltersNbr = 1;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 0;

	canCfg.rx_callback = NULL; // PLEASE SET
	canCfg.rx_interrupt_priority = 0; // PLEASE SET
	canCfg.tx_interrupt_priority = 0; // PLEASE SET
	canCfg.tx_buffer_length = 3; // PLEASE SET

	//canCfg.rx_gpio = GPIOB;
	//canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	//canCfg.tx_gpio = GPIOB;
	//canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	//canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;


    //Not testing filters at the moment
	//FDCAN_FilterTypeDef filter;

    //can_add_filter(can2Handle, &filter);
	/* USER CODE END 2 */

	FDCAN_TxHeaderTypeDef TxHeader = {
		.Identifier = 1,

		.IdType = FDCAN_STANDARD_ID,
		.TxFrameType = FDCAN_DATA_FRAME,
		.ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
			// FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
		.DataLength = 1,
		.BitRateSwitch = FDCAN_BRS_OFF,
		.TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
		.MessageMarker = 0 // also change this to a real address if you change fifo control
	};

	FDCANTxMessage msg;
	msg.data[0] = 0x80;
	memset(&(msg.data), 0, sizeof(msg.data));
	msg.tx_header = TxHeader;


	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

    #ifdef FDCAN1

	canCfg.fdcan_instance = FDCAN1;
    canCfg.rx_gpio = GPIOA;
    canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
    canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

    canCfg.tx_gpio = GPIOA;
    canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
    canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.rx_callback = can_test_rx_callback1; // PLEASE SET


	CANHandle *can1Handle = can_init(&canCfg);
	HAL_FDCAN_ConfigGlobalFilter(can1Handle->hal_fdcanP, 0, 0, 0,0);

	can_start(can1Handle);


    #endif
    #ifdef FDCAN2

    canCfg.fdcan_instance = FDCAN2;
    canCfg.rx_gpio = GPIOB;
    canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
    canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

    canCfg.tx_gpio = GPIOB;
    canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
    canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	canCfg.rx_callback = can_test_rx_callback2; 


	// FDCAN_FilterTypeDef filter;
	// filter.IdType = FDCAN_STANDARD_ID;
	// filter.FilterIndex = 0;
	// filter.FilterType = FDCAN_FILTER_RANGE,
	// filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	// filter.FilterID1 = 0x00;
	// filter.FilterID2 = 0x02;

    CANHandle *can2Handle = can_init(&canCfg);

    //accept unmatched standard and extended frames into RXFIFO0 - default behaviour
    HAL_FDCAN_ConfigGlobalFilter(can2Handle->hal_fdcanP, 0, 0, 0,0);

    //not accepting filters
    //can_add_filter(can2Handle, &filter);

    //API Testing
    //can_init(&canCfg);

    can_start(can2Handle);

    can_send(can2Handle, &msg);
    //can_release(can2Handle);

    #endif
    #ifdef FDCAN3

    #endif

	while (1) {
		HAL_Delay(1000);
		msg.data[0] = 0x2;
		can_send(can1Handle, &msg); 
		HAL_Delay(1000);
		msg.data[0] = 0x10;
		can_send(can2Handle, &msg); 
	}

    return 0;

}
