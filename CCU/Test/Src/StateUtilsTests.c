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

bool ACU_Warnings(const CCU_StateData *state_data)
{
	bool any_warnings = false;

	if (state_data->ACU_S2_UNDER20v_WARNING) {
		LOGOMATIC("Under 20v Warning\n");
		any_warnings = true;
	}
	if (state_data->ACU_S2_UNDER12v_WARNING) {
		LOGOMATIC("Under 12v Warning\n");
		any_warnings = true;
	}
	if (state_data->ACU_S2_UNDERVOLTSDC_WARNING) {
		LOGOMATIC("Undervolt TSDC Warning\n");
		any_warnings = true;
	}
	return any_warnings;
}

bool CriticalError(const CCU_StateData *state_data)
{
	bool any_error = false;

	if (state_data->ACU_S2_OVERCURR_ERROR) {
		LOGOMATIC("OVERCURR\n");
		any_error = true;
	}

	if (state_data->ACU_S2_OVERTEMP_ERROR) {
		LOGOMATIC("OVERTEMP\n");
		any_error = true;
	}

	if (state_data->ACU_S2_OVERVOLT_ERROR) {
		LOGOMATIC("OVERVOLT\n");
		any_error = true;
	}

	if (state_data->ACU_S2_UNDERCURR_ERROR) {
		LOGOMATIC("UNDERCURR\n");
		any_error = true;
	}

	if (state_data->ACU_S2_UNDERVOLT_ERROR) {
		LOGOMATIC("UNDERVOLT\n");
		any_error = true;
	}

	if (!state_data->IR_MINUS && state_data->IR_PLUS) {
		LOGOMATIC("IMPOSSIBLE IR STATE\n");
		any_error = true;
	}

	return any_error;
}

uint32_t MillisecondsSinceBoot(void)
{
	return HAL_GetTick() * HAL_GetTickFreq();
}
