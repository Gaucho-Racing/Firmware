#include "ecu_can.h"

#include "Unused.h"

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

int can_send(CANHandle *handle, FDCANTxMessage *buffer)
{
	UNUSED(handle);
	UNUSED(buffer);
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
