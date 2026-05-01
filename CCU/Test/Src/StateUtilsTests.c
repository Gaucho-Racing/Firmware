#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateUtils.h"
#include "gpio.h"
#include "stdint.h"

void TripSoftwareLatch(CCU_StateData *state_data)
{
	LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	state_data->SOFTWARE_LATCH = false;
	LOGOMATIC("Software Latch: Low\n");
}

bool BCU_Warnings(const CCU_StateData *state_data)
{
	if (state_data->BCU_S2_UNDER20v_WARNING) {
		LOGOMATIC("Under 20v Warning\n");
		return true;
	} else if (state_data->BCU_S2_UNDER12v_WARNING) {
		LOGOMATIC("Under 12v Warning\n");
		return true;
	} else if (state_data->BCU_S2_UNDERVOLTSDC_WARNING) {
		LOGOMATIC("Undervolt TSDC Warning\n");
		return true;
	} else {
		return false;
	}
}

bool CriticalError(const CCU_StateData *state_data)
{

	if (state_data->BCU_S2_OVERCURR_ERROR) {
		LOGOMATIC("OVERCURR\n");
		return true;

	} else if (state_data->BCU_S2_OVERTEMP_ERROR) {
		LOGOMATIC("OVERTEMP\n");
		return true;

	} else if (state_data->BCU_S2_OVERVOLT_ERROR) {
		LOGOMATIC("OVERVOLT\n");
		return true;

	} else if (state_data->BCU_S2_UNDERCURR_ERROR) {
		LOGOMATIC("UNDERCURR\n");
		return true;

	} else if (state_data->BCU_S2_UNDERVOLT_ERROR) {
		LOGOMATIC("UNDERVOLT\n");
		return true;

	} else if (!state_data->BCU_S2_IR_MINUS && state_data->BCU_S2_IR_PLUS) {
		LOGOMATIC("IMPOSSIBLE IR STATE\n");
		return true;

	} else {
		return false;
	}
}

uint32_t MillisecondsSinceBoot(void)
{
	return HAL_GetTick() * HAL_GetTickFreq();
}
