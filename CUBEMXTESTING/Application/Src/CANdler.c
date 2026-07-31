#include "CANdler.h"

#include "CubeMXCan.h"

void CANdler_Callback(CubeMXCan_Handle *const handle, void *const user_ctx)
{
	LOGOMATIC("CANdler_Callback: Received CAN message on handle %p with user context %p\n", (void *)handle, user_ctx);
	// TODO
}
