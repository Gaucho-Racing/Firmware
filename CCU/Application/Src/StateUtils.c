#include "StateUtils.h"

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

void setSoftwareLatch(bool close, CCU_StateData *state_data)
{

	if (close && !LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		state_data->BCU_S2_SOFTWARE_LATCH = 1;
		LOGOMATIC("Software Latch: High\n");
	} else if (!close && LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		state_data->BCU_S2_SOFTWARE_LATCH = 0;
		LOGOMATIC("Software Latch: Low\n");
	}
}

bool BCU_Warnings(const CCU_StateData *state_data)
{
	if (state_data->BCU_S2_UNDER20v_WARNING) {
		LOGOMATIC("Under 20v Warning");
		return true;
	} else if (state_data->BCU_S2_UNDER12v_WARNING) {
		LOGOMATIC("Under 12v Warning");
		return true;
	} else if (state_data->BCU_S2_UNDERVOLTSDC_WARNING) {
		LOGOMATIC("Undervolt TSDC Wanring");
		return true;
	} else {
		return false;
	}
}

bool CriticalError(const CCU_StateData *state_data)
{

	if (state_data->BCU_S2_OVERCURR_ERROR) {
		LOGOMATIC("OVERCURR");
		return true;

	} else if (state_data->BCU_S2_OVERTEMP_ERROR) {
		LOGOMATIC("OVERTEMP");
		return true;

	} else if (state_data->BCU_S2_OVERVOLT_ERROR) {
		LOGOMATIC("OVERVOLT");
		return true;

	} else if (state_data->BCU_S2_UNDERCURR_ERROR) {
		LOGOMATIC("UNDECURR");
		return true;

	} else if (state_data->BCU_S2_UNDERVOLT_ERROR) {
		LOGOMATIC("UNDEVOLT");
		return true;

	} else {
		return false;
	}
}
