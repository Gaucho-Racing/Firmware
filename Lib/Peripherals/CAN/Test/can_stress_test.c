#include <string.h>

#include "can.h"
#include "can_tests.h"



// TODO:
static volatile uint32_t can_stress_test_received = 0;
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
	LOGOMATIC("running can_stress_test\n");
	uint32_t status, loop;
	UNUSED(status);

	status = loop = 0;

	CANHandle *primary_can, *data_can;
	primary_can = data_can = NULL;
	CANConfig cfg1;

	if (get_cfg(FDCAN1, can_stress_test_rx_callback, &cfg1, FDCAN_MODE_INTERNAL_LOOPBACK)) {
		LOGOMATIC("Could not get config for FDCAN1\n");
		return ERROR;
	}

	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	//TODO: Make the stress test more stressful
	FDCAN_TxHeaderTypeDef TxHeader = {
	    .Identifier = 1,

	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
						     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	    .DataLength = 20,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
	};

	if ((primary_can = can_init(&cfg1)) == NULL) {
		LOGOMATIC("Could not initialize primary_can\n");
		return ERROR;
	}
	if (can_start(primary_can)) {
		LOGOMATIC("Could not start primary_can\n");
		return ERROR;
	}

	FDCANTxMessage msg;
	memset(&(msg.data), 0, sizeof(msg.data));
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	size_t i = 0;
	size_t rounds = 5;
	size_t messages = primary_can->tx_capacity * 2;
	uint32_t successes = 0;
	while (loop < rounds) {
		loop++;
		can_stress_test_received = 0;
		i = 0;
		while (i < messages) {
			if (can_send(primary_can, &msg) != 0) {
				LOGOMATIC("can_stress_test: FAIL: sending CAN msg at %u-th consecutive send.\n", (unsigned int)i + 1);
				break;
			}
			i++;
		}
		LOGOMATIC("Sent %u/%u CAN msgs...\n", (unsigned int)i, (unsigned int)messages);
		HAL_Delay(1000);

		LOGOMATIC("Received %u/%u CAN msgs after 1 second.\n", (unsigned int)can_stress_test_received, (unsigned int)messages);

		LOGOMATIC("finished loop %ld\n", loop);

		if (primary_can->tx_elements > 0) {
			LOGOMATIC("can_stress_test: FAIL: did not send all messages from tx_buffer\n");
			continue;
		}
		LOGOMATIC("\n");

		if (can_stress_test_received == messages) {
			successes += 1;
		}
		// msg.data[0] = 0x10;
		// can_send(data_can, &msg);
		// HAL_Delay(1000);
	}

	if (can_release(primary_can)) {
		LOGOMATIC("can_stress_test: FAIL: could not release primary_can\n");
		return ERROR;
	}

	// FINAL CHECK
	LOGOMATIC("can_stress_test: succeeded %u/%u rounds\n", (unsigned int)successes, (unsigned int)rounds);
	if (successes < rounds) {
		LOGOMATIC("can_stress_test: FAIL\n");
	} else {
		LOGOMATIC("can_stress_test: SUCCESS\n");
	}

	return SUCCESS;
}
