#include "can.h"
#include "can_tests.h"




int can_init_test(CANConfig *cfg)
{
	CANHandle *handle;
	if ((handle = can_init(cfg)) == NULL) {
		LOGOMATIC("can_init_test: init failed\n");
		return ERROR;
	}

	// inspect handle
	if (!handle->init) {
		LOGOMATIC("can_init_test: did not set init bool in handle\n");
		return ERROR;
	}

	return SUCCESS;
}
