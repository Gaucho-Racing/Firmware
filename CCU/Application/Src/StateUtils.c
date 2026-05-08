#include "StateUtils.h"

#include "CANdler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "Stringification.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"
#include "vcp.h"

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

	if (!state_data->ACU_S2_IR_MINUS && state_data->ACU_S2_IR_PLUS) {
		LOGOMATIC("IMPOSSIBLE IR STATE\n");
		any_error = true;
	}

	return any_error;
}

void VCP_Oneliner(const CCU_StateData *state_data)
{
	static char buffer[50]; // Static to avoid allocating 50 bytes on the stack every time this function is called
	uint8_t length = 0;

	length = snprintf(buffer, sizeof(buffer), "[%lu]", MillisecondsSinceBoot());
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " IR- %s", state_data->ACU_S2_IR_MINUS ? "Closed" : "Open");
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | IR+ %s", state_data->ACU_S2_IR_PLUS ? "Closed" : "Open");
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | %huV", state_data->Accumulator_Voltage / 100);
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | SOC %hu%%", (uint8_t)(state_data->Accumulator_SOC * 20.0f / 51.0f));
	VCP_Send(buffer, length);

	LOGOMATIC("\n--- State Bits ---\n");
	LOGOMATIC("SOFTWARE LATCH: %d\n", state_data->SOFTWARE_LATCH);
	length = snprintf(buffer, sizeof(buffer), " | Max Cell %huC", state_data->Max_Cell_Temp / 4);
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | %s", state_data->state == CCU_STATE_IDLE ? "IDLE" : "CHARGING");
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), "\n");
	VCP_Send(buffer, length);
}

uint32_t MillisecondsSinceBoot(void)
{
	return HAL_GetTick() * HAL_GetTickFreq();
}
