#include "can_tests.h"
#include "can.h"

//TODO: 
int can_release_test(CANHandle * handle) {
    if (handle == NULL) {return ERROR;}

    FDCAN_HandleTypeDef * hal_instance = handle->hal_fdcanP;

	if (can_release(handle)) LOGOMATIC("can_test; could not release primary_can\n");

    if (hal_instance != canHandle->hal_fdcanP) { LOGOMATIC("can_release_test; did not reset CAN handle correctly\n"); return ERROR;}

	return SUCCESS;
}
