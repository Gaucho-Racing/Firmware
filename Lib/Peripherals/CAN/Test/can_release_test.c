#include "can.h"
#include "can_tests.h"

// #define USECAN1
// #define TX_BUFFER_1_SIZE 10

// TODO:
int can_release_test()
{
	LOGOMATIC("running can_release_test\n");

	CANConfig cfg;
	if (get_cfg(FDCAN1, NULL, &cfg, FDCAN_MODE_NORMAL)) {
		LOGOMATIC("Could not get config for FDCAN1\n");
		return ERROR;
	}

	CANHandle *can = NULL;

	//=============================================================================================
	if ((can = can_init(&cfg)) == NULL) {
		LOGOMATIC("can_init: Could not initialize primary_can\n");
		return ERROR;
	}

	FDCAN_HandleTypeDef *temp = can->hal_fdcanP;
	if (can_release(can)) {
		LOGOMATIC("can_release: Could not release can\n");
		return ERROR;
	}

	// test state of canHandle after release
	if (temp != can->hal_fdcanP) {
		LOGOMATIC("can_release: cleared handle incorrectly\n");
		return ERROR;
	}

	return SUCCESS;
}
