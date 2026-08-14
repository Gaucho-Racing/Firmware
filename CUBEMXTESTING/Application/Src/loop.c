#include "loop.h"

#include "Logomatic.h"
#include "CubeCAN.h"
#include "main.h"
#include "GRCAN_NODE_ID.h"
#include "GRCAN_MSG_ID.h"

void MainLoop(void)
{
	LL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
	CubeCAN_Send(primaryHandle, GRCAN_Debugger, GRCAN_DEBUG_2_0, "Hello!", 6);
}
