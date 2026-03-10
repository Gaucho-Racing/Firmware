#include "can.h"
#include "can_tests.h"
#include <string.h>


//TODO:
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

	if ( (primary_can = can_init(&cfg1)) == NULL) {
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
	size_t messages = 5;
	while (i < messages) {
		loop++;
		can_stress_test_received = 0;
		i = 0;
		while (i < 100) {
			if (can_send(primary_can, &msg) != 0) {
				LOGOMATIC("Stress test failed sending CAN msg at %ud-th consecutive send.\n", (unsigned int) i + 1);
				break;
			}
			i++;
		}
		LOGOMATIC("Sent %ud CAN msgs...\n", (unsigned int)i);
		HAL_Delay(1000);

		LOGOMATIC("Received %ud/%ud CAN msgs after 1 second.\n", (unsigned int)can_stress_test_received, (unsigned int) i);
		msg.data[0] = 0x10;
		can_send(data_can, &msg);
		HAL_Delay(1000);
		LOGOMATIC("Stress test finished loop %ld\n", loop);
	}

	if (can_release(primary_can)) LOGOMATIC("can_test; could not release primary_can\n");

	return SUCCESS;
}
