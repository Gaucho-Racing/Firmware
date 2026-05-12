#include "Unused.h"
#include "can.h"

int can_start(CANHandle *handle)
{
	UNUSED(handle);
	return 0;
}

int can_stop(CANHandle *handle)
{
	UNUSED(handle);
	return 0;
}

int can_enqueue(CANHandle *canHandle, FDCANTxMessage *message)
{
	UNUSED(canHandle);
	UNUSED(message);
	return 0;
}

int can_release(CANHandle *handle)
{
	UNUSED(handle);
	return 0;
}

int can_add_filter(CANHandle *handle, FDCAN_FilterTypeDef *filter)
{
	UNUSED(handle);
	UNUSED(filter);
	return 0;
}
