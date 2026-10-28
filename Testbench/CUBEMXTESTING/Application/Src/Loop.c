#include "Loop.h"

#include <inttypes.h>

#include "CubeCAN.h"
#include "CubeVCP.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "main.h"

extern int value;

void MainLoop(void)
{
	LL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);

	(void)CubeCAN_Send(primaryHandle, GRCAN_Debugger, GRCAN_DEBUG_2_0, "Hello0", 6);
	(void)CubeCAN_Send(dataHandle, GRCAN_Debugger, GRCAN_DEBUG_2_0, "Hello1", 6);
	(void)CubeCAN_Send(chargerHandle, GRCAN_Debugger, GRCAN_DEBUG_2_0, "Hello2", 6);

	LOGOMATIC_VERBOSE("Main Loop %d %" PRIu32 "\n", value, HAL_GetTick());

	CubeVCP_SendString("Hello from VCP\n", sizeof("Hello from VCP\n") - 1);
}
