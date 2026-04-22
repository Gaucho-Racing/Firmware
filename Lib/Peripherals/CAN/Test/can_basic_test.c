#include <inttypes.h>
#include <string.h>

#include "can.h"
#include "can_tests.h"
#include "profile.h"

// #include "can_basic_test_cfg.h"

// CAN Configuration
// #define OLD_SAM

// define
#define LOOPBACK_MODE FDCAN_MODE_NORMAL
// #define RELEASE_AFTER_TEST
#ifdef RELEASE_AFTER_TEST
#pragma message "RELEASING CAN AFTER TESTS"
#endif

#define NODE_ID 2

#ifndef NODE_ID
#error "can_basic_test.c: please define node in can_basic_test_cfg.h"
#endif

// TODO: Decide how much of these parameters should go into config
#define NUM_NODES 2	// total number of nodes on the bus (including this one)
#define NUM_MESSAGES 20 // number of messages each node sends to every other node

#define CAN_PACKET_SIZE FDCAN_DLC_BYTES_64 // max is 64

#define NUM_TESTS FDCAN_DLC_BYTES_64
static float rx_stats[NUM_TESTS + 1] = {0};

// TODO: figure out if ifdef soup is avoidable
#ifdef FDCAN2
#define DATA_CAN
#endif

// FDCAN_DLC_BYTES_

// TODO: could make creating these callbacks a macro, rather than defining each one separately
static volatile uint32_t rx_1_received = 0;
static volatile uint8_t can1_data[64] = {0};
static void can_test_rx_callback1(uint32_t id, void *data, uint32_t size)
{
	dwt_timer_end_measurement(&rx_timer);

	rx_1_received++;
	LOGOMATIC("CAN1 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);
	// Is within an ISR, so needs to exit quickly
	uint8_t *data_bytes = (uint8_t *)data;

	// allow for cache coherency
	// for (int i = 0; i < 100000; i++);

	bool failure = false;
	for (uint32_t i = 0; i < size; i++) {
		can1_data[i] = data_bytes[i];

		if (can1_data[i] != i) {
			failure = true;
		}
	}

	// reset
	for (uint32_t i = 0; i < size; i++) {
		can1_data[i] = 0;
	}
	if (failure) {
		LOGOMATIC("FAIL: did not copy data correctly\n");
	}

	return;
}

#ifdef DATA_CAN
static volatile uint32_t rx_2_received = 0;
static void can_test_rx_callback2(uint32_t id, void *data, uint32_t size)
{
	rx_2_received++;
	LOGOMATIC("CAN2 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);

	// Is within an ISR, so needs to exit quickly
	return;
}
#endif

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??
int can_basic_test(void)
{
	FDCAN_TxHeaderTypeDef TxHeader = {
	    .Identifier = NODE_ID,
	    .FDFormat = FDCAN_FD_CAN,
	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
						     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	    .DataLength = CAN_PACKET_SIZE,
	    .BitRateSwitch = FDCAN_BRS_ON,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
	};

	CANHandle *primary_can = NULL;
	primary_can = NULL;
	CANConfig cfg1;

#ifdef DATA_CAN
	CANHandle *data_can = NULL;
	CANConfig cfg2;
#endif

	LOGOMATIC("Initializing primary and data CAN Bus in Normal mode.\n");
	if (get_cfg(FDCAN1, can_test_rx_callback1, &cfg1, LOOPBACK_MODE, 0, 0)) {
		LOGOMATIC("Could not get config for FDCAN1\n");
		return ERROR;
	}
	/*cfg1.rx_gpio = GPIOA;
	cfg1.init_rx_gpio.Pin = GPIO_PIN_11;
	cfg1.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	cfg1.tx_gpio = GPIOA;
	cfg1.init_tx_gpio.Pin = GPIO_PIN_12;
	cfg1.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;
	*/

#ifdef DATA_CAN
	if (get_cfg(FDCAN2, can_test_rx_callback2, &cfg2, LOOPBACK_MODE, 0, 0)) {
		LOGOMATIC("Could not get config for FDCAN2\n");
		return ERROR;
	}
#endif

#ifdef OLD_SAM
	cfg2.rx_gpio = GPIOB;
	cfg2.init_rx_gpio.Pin = GPIO_PIN_13;
	cfg2.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	cfg2.tx_gpio = GPIOB;
	cfg2.init_tx_gpio.Pin = GPIO_PIN_5;
	cfg2.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;
#endif

	set_default_can_clksource();


	//=============================================================================================
	if ((primary_can = can_init(&cfg1)) == NULL) {
		LOGOMATIC("Could not initialize primary_can\n");
		return ERROR;
	}
	HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, 0, 0, 0, 0);

#ifdef DATA_CAN
	if ((data_can = can_init(&cfg2)) == NULL) {
		LOGOMATIC("Could not initialize data_can\n");
		return ERROR;
	}
	HAL_FDCAN_ConfigGlobalFilter(data_can->hal_fdcanP, 0, 0, 0, 0);
#endif

	//=============================================================================================
	if (can_start(primary_can)) {
		LOGOMATIC("Could not start primary_can\n");
		return ERROR;
	}

#ifdef DATA_CAN
	if (can_start(data_can)) {
		LOGOMATIC("Could not start data_can\n");
		return ERROR;
	}
#endif

	FDCANTxMessage msg = {0};
	// msg.data[0] = 0x80;
	for (int i = 0; i < CANFD_DLCtoBytes[CAN_PACKET_SIZE]; i++) {
		msg.data[i] = i;
	}

	msg.tx_header = TxHeader;

	LOGOMATIC("Sending %d messages on each bus...\n", NUM_MESSAGES);

	// uint32_t node_target = 0;

	start_dwt();

	for (uint32_t data_length_code = 0; data_length_code < FDCAN_DLC_BYTES_64 + 1; data_length_code++) {
		msg.tx_header.DataLength = data_length_code;
		dwt_timer_t send1_timer = {0};
#ifdef DATA_CAN
		dwt_timer_t send2_timer = {0};
#endif

		dwt_timer_reset(&rx_timer);

		uint32_t i = 0;
		while (i < NUM_MESSAGES) {
			HAL_Delay(10);
			// msg.data[0] = 0x2;

			dwt_timer_start_measurement(&send1_timer);
			can_send(primary_can, &msg);
			dwt_timer_end_measurement(&send1_timer);

			HAL_Delay(10);
			// msg.data[0] = 0x10;

#ifdef DATA_CAN
			dwt_timer_start_measurement(&send2_timer);
			can_send(data_can, &msg);
			// for(int i = 0; i < 100; i++);
			dwt_timer_end_measurement(&send2_timer);
#endif

			i += 1;
		}

#ifdef PROFILE
		// LOGOMATIC("NORMAL MODE - timing entire Rx callback (not just fifo copy)\n");
		LOGOMATIC("CAN PACKET SIZE: %u\n", CANFD_DLCtoBytes[data_length_code]);
		LOGOMATIC("Send1 ===========\n");
		dwt_timer_print_info(&send1_timer);

#ifdef DATA_CAN
		LOGOMATIC("Send2 ===============\n");
		dwt_timer_print_info(&send2_timer);
#endif

		LOGOMATIC("Rx ===============\n");
		dwt_timer_print_info(&rx_timer);

		rx_stats[data_length_code] = dwt_timer_average_cycles(&rx_timer);

		LOGOMATIC("can_basic_test: SUCCESS\n");
#endif

		HAL_Delay(100);
		LOGOMATIC("Received %ld messages on bus1...\n", rx_1_received);

#ifdef DATA_CAN
		LOGOMATIC("Received %ld messages on bus2...\n", rx_2_received);
#endif

		uint32_t error = false;

		// TODO: Create testing functions to check state of can instance
		if (rx_1_received != NUM_MESSAGES * (NUM_NODES - 1)) {
			error = true;
			LOGOMATIC("FAIL: can_basic_test: did not receive all rx1\n");
		} else {
			LOGOMATIC("SUCCESS: can_basic_test: received all rx1\n");
		}
		rx_1_received = 0;

#ifdef DATA_CAN
		if ((rx_2_received != NUM_MESSAGES * (NUM_NODES - 1))) {
			error = true;
			LOGOMATIC("FAIL: can_basic_test: did not receive all rx2\n");
		} else {
			LOGOMATIC("SUCCESS: can_basic_test: received all rx2\n");
		}
		rx_2_received = 0;
#endif

		if (primary_can->tx_elements > 0) {
			LOGOMATIC("can_basic_test: FAIL: did not send all messages from primary tx_buffer\n");
		}

#ifdef DATA_CAN
		if (data_can->tx_elements > 0) {
			LOGOMATIC("can_basic_test: FAIL: did not send all messages from data tx_buffer\n");
		}
#endif

		LOGOMATIC("\n");
		UNUSED(error);
	}

	stop_dwt();

	LOGOMATIC("=================PROFILING RESULTS ============================\n");
	for (uint32_t i = 0; i <= FDCAN_DLC_BYTES_64; i++) {
		LOGOMATIC("%d: %.2f\n", CANFD_DLCtoBytes[i], rx_stats[i]);
	}
	LOGOMATIC("\n");

	uint32_t error = false;

#ifdef RELEASE_AFTER_TEST
	if ((error |= can_release(primary_can))) {
		LOGOMATIC("FAIL: can_basic_test; could not release primary_can\n");
	}

#ifdef DATA_CAN
	if ((error |= can_release(data_can))) {
		LOGOMATIC("FAIL: can_basic_test; could not release data_can\n");
	}
#endif
#endif

#ifndef RELEASE_AFTER_TEST
	while (true) {
		can_send(primary_can, &msg);
		HAL_Delay(100);
	}
#endif

	if (error) {
		return ERROR;
	}

	return SUCCESS;
}
