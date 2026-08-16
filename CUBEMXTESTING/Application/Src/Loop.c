#include "Loop.h"

#include "CubeCAN.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "main.h"

extern int value;

void MainLoop(void)
{
	LL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
	LOGOMATIC("Main Loop %d\n", value);
	(void)CubeCAN_Send(primaryHandle, GRCAN_Debugger, GRCAN_DEBUG_2_0, "Hello!", 6);
}
