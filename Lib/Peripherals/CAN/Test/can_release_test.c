#include "can.h"
#include "can_tests.h"
#include "Logomatic.h"


static volatile uint32_t rx_1_received = 0;
static void can_release_callback1(uint32_t id, void *data, uint32_t size)
{
	rx_1_received++;
	LOGOMATIC("CAN1 Got data! Size %ld, data[0] = 0x%x, id %ld\n", size, *(char *)data, id);
	return;
}
// TODO:
int can_release_test()
{
	LOGOMATIC("running can_release_test\n");

	CANConfig cfg;
	if (get_cfg(FDCAN1, can_release_callback1, &cfg, FDCAN_MODE_INTERNAL_LOOPBACK,0,0)) {
		LOGOMATIC("Could not get config for FDCAN1\n");
		return ERROR;
	}

	CANHandle *can = NULL;

	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	//=============================================================================================
	if ((can = can_init(&cfg)) == NULL) {
		LOGOMATIC("can_init: FAIL Could not initialize primary_can\n");
		return ERROR;
	}

	//send a message =======================
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

	FDCANTxMessage msg = {0};
	msg.data[0] = 0x80;
	msg.tx_header = TxHeader;

	can_start(can);
	can_send(can, &msg);
	HAL_Delay(500);
	if (rx_1_received != 1) {
		LOGOMATIC("can_release: FAIL: did not receive message over loopback\n");
		return ERROR;
	}

	//Test Releasing
	FDCAN_HandleTypeDef *temp = can->hal_fdcanP;
	uint32_t cap = can->tx_capacity;
	FDCANTxMessage* buff = can->tx_buffer;
	if (can_release(can)) {
		LOGOMATIC("can_release: FAIL: Could not release can\n");
		return ERROR;
	}

	//TODO: use a stack canary to see if memory was cleared safely??
	// test state of canHandle after release
	if (temp != can->hal_fdcanP ||
		cap != can->tx_capacity ||
		buff != can->tx_buffer ||
		can->init ||
		can->started ||
		can->tx_elements ||
		can->tx_tail ||
		can->rx_callback) {
		LOGOMATIC("can_release: FAIL: cleared handle incorrectly\n");
		return ERROR;
	}

	//Do it again for good luck
	if ((can = can_init(&cfg)) == NULL) {
		LOGOMATIC("can_init: FAIL Could not initialize primary_can the second time\n");
		return ERROR;
	}
	temp = can->hal_fdcanP;
	cap = can->tx_capacity;
	buff = can->tx_buffer;
	if (can_release(can)) {
		LOGOMATIC("can_release: FAIL: Could not release can the second time\n");
		return ERROR;
	}
	if (temp != can->hal_fdcanP ||
		cap != can->tx_capacity ||
		buff != can->tx_buffer ||
		can->init ||
		can->started ||
		can->tx_elements ||
		can->tx_tail ||
		can->rx_callback) {
		LOGOMATIC("can_release: FAIL: cleared handle incorrectly\n");
		return ERROR;
	}

	LOGOMATIC("can_release: SUCCESS\n");
	return SUCCESS;
}
