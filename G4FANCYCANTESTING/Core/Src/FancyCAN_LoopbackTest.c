#include <inttypes.h>
#include <string.h>

#include "Logomatic.h"
#include "grcan_fancylayer.h"
#include "stdlib.h"

#include <assert.h>

// CAN Configuration
// #define OLD_SAM

#if defined(OLD_SAM)
#define NODE_ID 1 // change for each node you flash
#else
#define NODE_ID 2
#endif

// Static variable to track received messages
static volatile uint32_t rx_received = 0;

// Static variable to track if the received data matches the expected data
static volatile bool data_valid = false;
static uint8_t expected_data[] = "Hello";

// Updated callback function to verify received data
static void can_test_rx_callback(uint32_t id, void *data, uint32_t size) {
    rx_received++;
    if (size == sizeof(expected_data) && memcmp(data, expected_data, size) == 0) {
        data_valid = true;
    } else {
        data_valid = false;
    }
    LOGOMATIC("\nCallback triggered: ID=%" PRIu32 ", Size=%ld, Data[0]=0x%x\n", id, size, *(uint8_t *)data);
}

// #define NUM_NODES 2    // total number of nodes on the bus (including this one)
// #define NUM_MESSAGES 5 // number of messages each node sends to every other node

// TODO: could make creating these callbacks a macro, rather than defining each one separately
// static volatile uint32_t rx_2_received = 0;
// static void can_test_rx_callback2(uint32_t id, void *data, uint32_t size)
// {
// 	rx_2_received++;
// 	LOGOMATIC("CAN2 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);
// 	// Is within an ISR, so needs to exit quickly
// 	return;
// }

// static volatile uint32_t rx_1_received = 0;
// static void can_test_rx_callback1(uint32_t id, void *data, uint32_t size)
// {
// 	rx_1_received++;
// 	LOGOMATIC("CAN1 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);

// 	// Is within an ISR, so needs to exit quickly
// 	return;
// }

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??

int GRCAN_Test_InitBus() {
    GRCAN_BusConfig bus_config;
    GRCAN_SetDefaultBusConfig(&bus_config, GRCAN_BUS_TESTING);
    bus_config.operating_mode = GRCAN_OPMODE_INTERNAL_LOOPBACK;
    bus_config.fdcan_instance = FDCAN2;
	bus_config.rx_callback = can_test_rx_callback; //test loopback

    LOGOMATIC("Testing GRCAN_InitBus...\n");
    bool result = GRCAN_InitBus(&bus_config);
    if (result == true) {
        LOGOMATIC("GRCAN_InitBus PASSED.\n");
		return 1;
    }
    else {
        LOGOMATIC("GRCAN_InitBus FAILED.\n");
		return 0;
    }
}


int GRCAN_Test_SendReceive() {
    GRCAN_SetLocalNodeID(1);

    uint8_t data[] = "Hello";
    memcpy(expected_data, data, sizeof(data)); // Set the expected data
    GRCAN_Fancy_Send(GRCAN_BUS_TESTING, 2, 0x12, data, sizeof(data));

    // Wait for the callback to be triggered
    HAL_Delay(1000); // Simulate waiting for message processing

    // Verify the callback was triggered and data is valid
    if (rx_received > 0 && data_valid) {
        LOGOMATIC("GRCAN_Fancy_Send PASSED. Callback verified and data is valid.\n");
        return 1;
    } else {
        LOGOMATIC("GRCAN_Fancy_Send FAILED. Callback not triggered or data is invalid.\n");
        return 0;
    }
}

int GRCAN_Test_ErrorHandling() {
    GRCAN_BusConfig invalid_config = {0};

    LOGOMATIC("\nTesting GRCAN_InitBus with invalid config...\n");
    bool result = GRCAN_InitBus(&invalid_config);
    if (result == false) {
        LOGOMATIC("GRCAN_InitBus error handling PASSED.\n");
		return 1;
    }
    else {
       LOGOMATIC("GRCAN_InitBus error handling FAILED.\n");
	   return 0;
    }
}

int FancyCAN_LoopbackTest(void)
{

    int res1 = GRCAN_Test_InitBus();
    int res2 = GRCAN_Test_SendReceive();
    int res3 = GRCAN_Test_ErrorHandling();

    return res1 == 1 && res2 == 1 && res3 == 1;

    // TODO: actually check if the message was received correctly and return true if so

	// FDCAN_TxHeaderTypeDef TxHeader = {
	//     .Identifier = NODE_ID,

	//     .IdType = FDCAN_STANDARD_ID,
	//     .TxFrameType = FDCAN_DATA_FRAME,
	//     .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
	// 					     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	//     .DataLength = 1,
	//     .BitRateSwitch = FDCAN_BRS_OFF,
	//     .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	//     .MessageMarker = 0			      // also change this to a real address if you change fifo control
	// };

	// CANHandle *primary_can = 0, *data_can = 0;
	// primary_can = data_can = NULL;
	// CANConfig cfg1, cfg2;

	// LOGOMATIC("Initializing primary and data CAN Bus in Normal mode.\n");
	// if (get_cfg(FDCAN1, can_test_rx_callback1, &cfg1, FDCAN_MODE_NORMAL, 0, 0)) {
	// 	LOGOMATIC("Could not get config for FDCAN1\n");
	// 	return ERROR;
	// }
	/*cfg1.rx_gpio = GPIOA;
	cfg1.init_rx_gpio.Pin = GPIO_PIN_11;
	cfg1.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	cfg1.tx_gpio = GPIOA;
	cfg1.init_tx_gpio.Pin = GPIO_PIN_12;
	cfg1.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;
	*/

	// 	if (get_cfg(FDCAN2, can_test_rx_callback2, &cfg2, FDCAN_MODE_NORMAL, 0, 0)) {
	// 		LOGOMATIC("Could not get config for FDCAN2\n");
	// 		return ERROR;
	// 	}
	// #ifdef OLD_SAM
	// 	cfg2.rx_gpio = GPIOB;
	// 	cfg2.init_rx_gpio.Pin = GPIO_PIN_13;
	// 	cfg2.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// 	cfg2.tx_gpio = GPIOB;
	// 	cfg2.init_tx_gpio.Pin = GPIO_PIN_5;
	// 	cfg2.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;
	// #endif

	// 	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	// 	//=============================================================================================
	// 	if ((primary_can = can_init(&cfg1)) == NULL) {
	// 		LOGOMATIC("Could not initialize primary_can\n");
	// 		return ERROR;
	// 	}
	// 	HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, 0, 0, 0, 0);

	// 	if ((data_can = can_init(&cfg2)) == NULL) {
	// 		LOGOMATIC("Could not initialize data_can\n");
	// 		return ERROR;
	// 	}
	// 	HAL_FDCAN_ConfigGlobalFilter(data_can->hal_fdcanP, 0, 0, 0, 0);

	// 	//=============================================================================================
	// 	if (can_start(primary_can)) {
	// 		LOGOMATIC("Could not start primary_can\n");
	// 		return ERROR;
	// 	}
	// 	if (can_start(data_can)) {
	// 		LOGOMATIC("Could not start data_can\n");
	// 		return ERROR;
	// 	}

	// 	FDCANTxMessage msg = {0};
	// 	msg.data[0] = 0x80;
	// 	msg.tx_header = TxHeader;

	// 	LOGOMATIC("Sending %d messages on each bus...\n", NUM_MESSAGES);

	// 	// uint32_t node_target = 0;
	// 	uint32_t i = 0;

	// 	while (i < NUM_MESSAGES) {
	// 		HAL_Delay(100);
	// 		msg.data[0] = 0x2;
	// 		can_send(primary_can, &msg);
	// 		HAL_Delay(100);
	// 		msg.data[0] = 0x10;
	// 		can_send(data_can, &msg);
	// 		i += 1;
	// 	}

	// 	HAL_Delay(5000);
	// 	LOGOMATIC("Received %ld messages on bus1...\n", rx_1_received);
	// 	LOGOMATIC("Received %ld messages on bus2...\n", rx_2_received);

	// 	uint32_t error = false;

	// 	// TODO: Create testing functions to check state of can instance
	// 	if (rx_1_received != NUM_MESSAGES * (NUM_NODES - 1)) {
	// 		error = true;
	// 		LOGOMATIC("FAIL: can_external_test: did not receive all rx1\n");
	// 	} else {
	// 		LOGOMATIC("SUCCESS: can_external_test: received all rx1\n");
	// 	}
	// 	if ((rx_2_received != NUM_MESSAGES * (NUM_NODES - 1))) {
	// 		error = true;
	// 		LOGOMATIC("FAIL: can_external_test: did not receive all rx2\n");
	// 	} else {
	// 		LOGOMATIC("SUCCESS: can_external_test: received all rx2\n");
	// 	}

	// 	if (primary_can->tx_elements > 0) {
	// 		LOGOMATIC("can_external_test: FAIL: did not send all messages from primary tx_buffer\n");
	// 	}
	// 	if (data_can->tx_elements > 0) {
	// 		LOGOMATIC("can_external_test: FAIL: did not send all messages from data tx_buffer\n");
	// 	}
	// 	LOGOMATIC("\n");

	// 	uint32_t rc;
	// 	if ((rc = can_release(primary_can))) {
	// 		LOGOMATIC("FAIL: can_external_test; could not release primary_can\n");
	// 	}
	// 	error |= rc;
	// 	if ((rc = can_release(data_can))) {
	// 		LOGOMATIC("FAIL: can_external_test; could not release data_can\n");
	// 	}
	// 	error |= rc;

	// 	if (error) {
	// 		return ERROR;
	// 	}

	// 	LOGOMATIC("can_external_test: SUCCESS\n");

	return SUCCESS;
}
