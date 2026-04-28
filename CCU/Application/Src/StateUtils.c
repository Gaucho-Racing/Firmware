#include "StateUtils.h"

#include "CANdler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"
#include "vcp.h"
#include "Stringification.h"

void setSoftwareLatch(CCU_StateData *state_data)
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

	} else {
		return false;
	}
}

void VCP_Oneliner(const CCU_StateData* state_data)
{
	static char buffer[50];	// Static to avoid allocating 50 bytes on the stack every time this function is called
	uint8_t length = 0;

	length = snprintf(buffer, sizeof(buffer), "[%lu]" , MillisecondsSinceBoot());
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " IR- %s", state_data->BCU_S2_PRECHARGE_STATE ? "Closed" : "Open");
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | IR+ %s", state_data->BCU_S2_IR_STATE ? "Closed" : "Open");
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | %huV", state_data->Accumulator_Voltage / 100);
	VCP_Send(buffer, length);

	length = snprintf(buffer, sizeof(buffer), " | SOC %hu%%", (uint8_t)(state_data->Accumulator_SOC * 20.0f / 51.0f));
	VCP_Send(buffer, length);

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

bool IR_Sanity_Check(CCU_StateData *state_data)
{

	if (state_data->BCU_S2_PRECHARGE_STATE && !state_data->BCU_S2_IR_STATE) {
		if (state_data->CCU_PRECHARGE_SET_TS_ACTIVE == false) {
			LOGOMATIC("IR- is closed but Precharge is not active. This should not be possible.");
			return false;
		} else {
			LOGOMATIC("Precharge in progress"); // don't know if this logging is necessary, but doing for debugging rn
			return true;
		}

	} else if (!state_data->BCU_S2_PRECHARGE_STATE && state_data->BCU_S2_IR_STATE) {
		LOGOMATIC("This shouldn't be possible");
		return false;
	} else if (!state_data->BCU_S2_PRECHARGE_STATE && !state_data->BCU_S2_IR_STATE) {
		LOGOMATIC("Not charging"); // don't know if this logging is necessary, but doing for debugging rn
		return true;
	} else if (state_data->BCU_S2_PRECHARGE_STATE && state_data->BCU_S2_IR_STATE) {
		LOGOMATIC("Charging should be complete");
		return true;
	} else {
		LOGOMATIC("Unknown case has occurred");
		return false;
	}
}
