#include "can.h"

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

uint8_t BytesToCANDLC(uint32_t num_bytes)
{
	UNUSED(num_bytes);
	return 0;
}
