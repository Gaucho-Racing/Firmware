#include "can_tests.h"

#include <inttypes.h>
#include <string.h>

#include "can.h"

// each family has a constant number of CAN peripherals

int can_test_instance(FDCAN_HandleTypeDef fdcan_handle)
{
	UNUSED(fdcan_handle);
	return 0;
}

void can_test_rx_callback2(uint32_t id, void *data, uint32_t size)
{
	LOGOMATIC("CAN2 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);
	// Is within an ISR, so needs to exit quickly
	return;
}

void can_test_rx_callback1(uint32_t id, void *data, uint32_t size)
{
	LOGOMATIC("CAN1 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);

	// Is within an ISR, so needs to exit quickly
	return;
}

int defaultCANCfg(CAN_RXCallback primary_callback, CAN_RXCallback data_callback, CANHandle **primary_can_out, CANHandle **data_can_out)
{
	CANConfig canCfg;
	// canCfg.fdcan_instance = FDCAN2;

	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
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

	canCfg.rx_callback = NULL;	  // PLEASE SET
	canCfg.rx_interrupt_priority = 5; // PLEASE SET
	canCfg.tx_interrupt_priority = 5; // PLEASE SET

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

	// Not testing filters at the moment
	// FDCAN_FilterTypeDef filter;

	// can_add_filter(data_can, &filter);
	/* USER CODE END 2 */

#ifdef USECAN1
	canCfg.fdcan_instance = FDCAN1;
	canCfg.rx_gpio = GPIOA;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.tx_gpio = GPIOA;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.rx_callback = primary_callback; // PLEASE SET

	CANHandle *primary_can = can_init(&canCfg);
	if (primary_can == NULL) {
		return -1;
	}
	*primary_can_out = primary_can;

	HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, 0, 0, 0, 0);

	can_start(primary_can);

#endif
#ifdef USECAN2
	canCfg.fdcan_instance = FDCAN2;
	canCfg.rx_gpio = GPIOB;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	canCfg.tx_gpio = GPIOB;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	canCfg.rx_callback = data_callback;

	// FDCAN_FilterTypeDef filter;
	// filter.IdType = FDCAN_STANDARD_ID;
	// filter.FilterIndex = 0;
	// filter.FilterType = FDCAN_FILTER_RANGE,
	// filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	// filter.FilterID1 = 0x00;
	// filter.FilterID2 = 0x02;

	CANHandle *data_can = can_init(&canCfg);
	if (data_can == NULL) {
		return ERROR;
	}
	*data_can_out = data_can;

	// accept unmatched standard and extended frames into RXFIFO0 - default behaviour
	if (HAL_FDCAN_ConfigGlobalFilter(data_can->hal_fdcanP, 0, 0, 0, 0) != HAL_OK) {
		return ERROR;
	}

	// not accepting filters
	// can_add_filter(data_can, &filter);

	// API Testing
	// can_init(&canCfg);

	if (can_start(data_can)) {
		return ERROR;
	}
	// can_release(data_can);

#endif
#ifdef USECAN3
#endif

	return SUCCESS;
}

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration
int can_test(void)
{
	FDCAN_TxHeaderTypeDef TxHeader = {
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

	CANHandle *primary_can, *data_can;
	primary_can = data_can = NULL;

	if (defaultCANCfg(can_test_rx_callback1, can_test_rx_callback2, &primary_can, &data_can)) {
		LOGOMATIC("Could not initialize and start both can buses");
		return ERROR;
	}

	FDCANTxMessage msg = {0};
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	while (1) {
		HAL_Delay(1000);
		msg.data[0] = 0x2;
		can_send(primary_can, &msg);
		HAL_Delay(1000);
		msg.data[0] = 0x10;
		can_send(data_can, &msg);
	}

	return 0;
}

static int can_stress_test_received = 0;
void can_stress_test_rx_callback(uint32_t id, void *data, uint32_t size)
{
	can_stress_test_received++;
	UNUSED(id);
	UNUSED(data);
	UNUSED(size);
	return;
}

int can_stress_test(void)
{
	int status, i, loop;
	UNUSED(status);

	status = loop = 0;

	CANHandle *primary_can, *data_can;
	primary_can = data_can = NULL;

	defaultCANCfg(can_stress_test_rx_callback, NULL, &primary_can, &data_can);

	FDCAN_TxHeaderTypeDef TxHeader = {
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

	FDCANTxMessage msg;
	memset(&(msg.data), 0, sizeof(msg.data));
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	while (1) {
		loop++;
		can_stress_test_received = 0;
		i = 0;
		while (i < 100) {
			if (can_send(primary_can, &msg) != 0) {
				LOGOMATIC("Stress test failed sending CAN msg at %d-th consecutive send.\n", i + 1);
				break;
			}
			i++;
		}
		LOGOMATIC("Sent %d CAN msgs...\n", i);
		HAL_Delay(1000);

		LOGOMATIC("Received %d/%d CAN msgs after 1 second.\n", can_stress_test_received, i);
		msg.data[0] = 0x10;
		can_send(data_can, &msg);
		HAL_Delay(1000);
		LOGOMATIC("Stress test finished loop %d\n", loop);
	}

	return 0;
}
