#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/common.h"

const char *CAN_GetInstanceName(FDCAN_GlobalTypeDef *instance)
{
	if (instance == NULL) {
		return "NULL";
	}
#if defined(STM32G474xx)
	if (instance == FDCAN1) {
		return "FDCAN1";
	} else if (instance == FDCAN2) {
		return "FDCAN2";
	} else if (instance == FDCAN3) {
		return "FDCAN3";
	}
#elif defined(STM32G431xx)
	if (instance == FDCAN1) {
		return "FDCAN1";
	}
#endif
	LOGOMATIC("Get CAN instance name: unknown instance\n");
	return "UNKNOWN";
}
