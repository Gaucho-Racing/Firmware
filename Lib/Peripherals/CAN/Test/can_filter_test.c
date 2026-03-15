#include <string.h>

#include "can_tests.h"

#define ID 20

// TODO:
//  TODO:
static volatile uint32_t can_filter_test_received = 0;
void can_filter_test_rx_callback(uint32_t id, void *data, uint32_t size)
{
	can_filter_test_received++;
	UNUSED(id);
	UNUSED(data);
	UNUSED(size);
	return;
}

void sendMessages(CANHandle *handle, FDCANTxMessage *msg)
{
	size_t i = 0;
	size_t messages = handle->tx_capacity;
	can_filter_test_received = 0;
	i = 0;

	// send some messages, shouldn't receive any because of global filter
	while (i < messages) {
		if (can_send(handle, msg) != 0) {
			LOGOMATIC("can_filter_test: FAIL: sending CAN msg at %u-th consecutive send.\n", (unsigned int)i + 1);
			break;
		}
		i++;
	}
	LOGOMATIC("Sent %u/%u CAN msgs...\n", (unsigned int)i, (unsigned int)messages);
	HAL_Delay(1000);

	LOGOMATIC("Received %u/%u CAN msgs after 1 second.\n", (unsigned int)can_filter_test_received, (unsigned int)messages);
	if (handle->tx_elements > 0) {
		LOGOMATIC("can_filter_test: FAIL: did not send all messages from tx_buffer\n");
	}
	// LOGOMATIC("\n");
}

// TODO: Filter test with multiple FIFOs
int can_filter_test(void)
{
	LOGOMATIC("running can_filter_test\n");
	uint32_t status, loop;
	UNUSED(status);

	status = loop = 0;

	CANHandle *primary_can, *data_can;
	primary_can = data_can = NULL;
	CANConfig cfg1;

	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	if (get_cfg(FDCAN1, can_filter_test_rx_callback, &cfg1, FDCAN_MODE_INTERNAL_LOOPBACK, 1, 0)) {
		LOGOMATIC("can_filter_test: FAIL, could not get config for FDCAN1\n");
		return ERROR;
	}

	//==================================================
	// cfg1.hal_fdcan_init.StdFiltersNbr = 1;
	//==================================================
	if ((primary_can = can_init(&cfg1)) == NULL) {
		LOGOMATIC("can_add_filter: FAIL, could not initialize primary_can\n");
		return ERROR;
	}

	// HAL_FDCAN_ConfigFilter(primary_can->hal_fdcanP, &fdcan1_filter);

	// reject all non-matching
	// HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

	FDCAN_TxHeaderTypeDef TxHeader = {
	    .Identifier = ID,

	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
						     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	    .DataLength = 1,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
	};

	if (can_start(primary_can)) {
		LOGOMATIC("can_add_filter: FAIL, could not start primary_can\n");
		return ERROR;
	}

	FDCANTxMessage msg;
	memset(&(msg.data), 0, sizeof(msg.data));
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	//=======================TEST UNFILTERED=====================
	LOGOMATIC("can_filter_test: TESTING: sending unfiltered messages\n");
	sendMessages(primary_can, &msg);
	if (can_filter_test_received > 0) {
		LOGOMATIC("can_filter_test: SUCCESS: received unfiltered messages\n");
	} else {
		LOGOMATIC("can_filter_test: FAIL: didn't receive unfiltered messages\n");
	}

	if (can_stop(primary_can)) {
		LOGOMATIC("can_add_filter: FAIL, could not stop primary_can\n");
		return ERROR;
	}
	LOGOMATIC("\n");

	//===================== TEST GLOBAL REJECT FILTER===============================
	LOGOMATIC("can_filter_test: TESTING: adding global reject filter\n");
	HAL_FDCAN_ConfigGlobalFilter(primary_can->hal_fdcanP, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
	if (can_start(primary_can)) {
		LOGOMATIC("can_add_filter: FAIL, could not start primary_can\n");
		return ERROR;
	}

	sendMessages(primary_can, &msg);
	if (can_filter_test_received > 0) {
		LOGOMATIC("can_filter_test: FAIL: global reject filter didn't work\n");
		return ERROR;
	} else {
		LOGOMATIC("can_filter_test: SUCCESS: global reject filter worked\n");
	}

	if (can_stop(primary_can)) {
		LOGOMATIC("can_add_filter: FAIL, could not stop primary_can\n");
		return ERROR;
	}
	LOGOMATIC("\n");

	//======================TEST STANDARD ACCEPT FILTER======================
	LOGOMATIC("can_filter_test: TESTING: adding standard accept filter\n");
	// Adding filter
	//  Filter 1 Definitions
	FDCAN_FilterTypeDef fdcan1_filter;

	fdcan1_filter.IdType = FDCAN_STANDARD_ID;
	fdcan1_filter.FilterIndex = 0;
	fdcan1_filter.FilterType = FDCAN_FILTER_MASK;
	fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan1_filter.FilterID1 = ID;
	fdcan1_filter.FilterID2 = 0x00000FF;
	if (can_add_filter(primary_can, &fdcan1_filter) != CAN_SUCCESS) {
		LOGOMATIC("can_filter_test: FAIL, failed to add filter\n");
		return ERROR;
	}

	if (can_start(primary_can)) {
		LOGOMATIC("can_add_filter: FAIL, could not start primary_can\n");
		return ERROR;
	}

	sendMessages(primary_can, &msg);
	if (can_filter_test_received > 0) {
		LOGOMATIC("can_filter_test: SUCCESS: standard accept filter worked\n");
	} else {
		LOGOMATIC("can_filter_test: FAIL: standard accept filter didn't work\n");
		return ERROR;
	}

	if (can_release(primary_can)) {
		LOGOMATIC("can_filter_test: FAIL: could not release primary_can\n");
		return ERROR;
	}

	LOGOMATIC("can_filter_test: SUCCESS: passed all tests!\n");

	return SUCCESS;
}
