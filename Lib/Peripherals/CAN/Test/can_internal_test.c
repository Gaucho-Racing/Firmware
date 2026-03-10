#include <inttypes.h>
#include <string.h>

#include "can.h"
#include "can_tests.h"

// each family has a constant number of CAN peripherals

/*int can_test_instance(FDCAN_HandleTypeDef fdcan_handle)
{
	//can_init(fdcan_handle);
	UNUSED(fdcan_handle);
	return 0;
}*/

// TODO: Make creating these callbacks a macro, rather than defining each one separately
static volatile int rx_2_received = 0;
void can_test_rx_callback2(uint32_t id, void *data, uint32_t size)
{
	rx_2_received++;
	LOGOMATIC("CAN2 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);
	// Is within an ISR, so needs to exit quickly
	return;
}

static volatile int rx_1_received = 0;
void can_test_rx_callback1(uint32_t id, void *data, uint32_t size)
{
	rx_1_received++;
	LOGOMATIC("CAN1 Got data! Size %ld, data[0] = 0x%x, id %" PRIu32 "\n", size, *(char *)data, id);

	// Is within an ISR, so needs to exit quickly
	return;
}

// TODO - allow user to send data without needing to construct a header for the buffer
//  TODO: G4 tests are dependent on the System clock configuration??
int can_external_test(void)
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

	CANHandle *primary_can = 0, *data_can = 0;
	primary_can = data_can = NULL;
	CANConfig cfg1, cfg2;

	LOGOMATIC("Initializing primary and data CAN Bus in Normal mode.\n");
	if (get_cfg(FDCAN1, can_test_rx_callback1, &cfg1, FDCAN_MODE_INTERNAL_LOOPBACK)) {
		LOGOMATIC("Could not get config for FDCAN1\n");
		return ERROR;
	}
	if (get_cfg(FDCAN2, can_test_rx_callback2, &cfg2, FDCAN_MODE_INTERNAL_LOOPBACK)) {
		LOGOMATIC("Could not get config for FDCAN2\n");
		return ERROR;
	}

	//=============================================================================================
	if ((primary_can = can_init(&cfg1)) == NULL) {
		LOGOMATIC("Could not initialize primary_can\n");
		return ERROR;
	}
	HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, 0, 0, 0, 0);

	if ((data_can = can_init(&cfg2)) == NULL) {
		LOGOMATIC("Could not initialize data_can\n");
		return ERROR;
	}
	HAL_FDCAN_ConfigGlobalFilter(data_can->hal_fdcanP, 0, 0, 0, 0);

	//=============================================================================================
	if (can_start(primary_can)) {
		LOGOMATIC("Could not start primary_can\n");
		return ERROR;
	}
	if (can_start(data_can)) {
		LOGOMATIC("Could not start data_can\n");
		return ERROR;
	}

	FDCANTxMessage msg = {0};
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	uint32_t i = 0;
	uint32_t num_messages = 5;

	LOGOMATIC("Sending %ld messages on each bus...\n", num_messages);
	while (i < num_messages) {
		HAL_Delay(1000);
		msg.data[0] = 0x2;
		can_send(primary_can, &msg);
		HAL_Delay(1000);
		msg.data[0] = 0x10;
		can_send(data_can, &msg);
		i += 1;
	}
	LOGOMATIC("Received %d messages on bus1...\n", rx_1_received);
	LOGOMATIC("Received %d messages on bus2...\n", rx_2_received);

	if (can_release(primary_can)) {
		LOGOMATIC("can_test; could not release primary_can\n");
	}
	if (can_release(data_can)) {
		LOGOMATIC("can_test; could not release data_can\n");
	}

	return SUCCESS;
}
