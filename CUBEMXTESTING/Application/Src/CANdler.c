#include "CANdler.h"

#include "CubeCAN.h"

void CANdler_Callback(CubeCAN_Handle *const handle, void *const user_context)
{
	LOGOMATIC("CANdler_Callback: Received CAN message on handle %p with user context %p\n", (void *)handle, user_context);
	// TODO
}
