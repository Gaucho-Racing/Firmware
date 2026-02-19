#include "StateUtils.h"

#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

void setSoftwareLatch(bool close)
{

	if (close && !(HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))) {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
		state_data.BCU_S2_SOFTWARE_LATCH = 1;
		LOGOMATIC("Software Latch: High\n");
	} else if (!(close) && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
		state_data.BCU_S2_SOFTWARE_LATCH = 0;
		LOGOMATIC("Software Latch: Low\n");
	}
}

bool CriticalError(const CCU_StateData *state_data)
{

	if (state_data->BCU_S2_OVERCURR_ERROR) {
		return true;

	} else if (state_data->BCU_S2_OVERTEMP_ERROR) {
		return true;

	} else if (state_data->BCU_S2_OVERVOLT_ERROR) {
		return true;

	} else if (state_data->BCU_S2_UNDERCURR_ERROR) {
		return true;

	} else if (state_data->BCU_S2_UNDERVOLT_ERROR) {
		return true;

	} else {
		return false;
	}
}
