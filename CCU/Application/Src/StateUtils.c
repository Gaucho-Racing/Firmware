#include "StateUtils.h"

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

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

void CheckDebuggerPrint(CCU_StateData *state_data)
{
	if (!(state_data->request_print_statedata)) {
		return;
	}

	LOGOMATIC("\n========== CCU STATE DUMP ==========\n");

	LOGOMATIC("state: %d\n", state_data->state);
	LOGOMATIC("recv_charge_cmd: %d\n", state_data->recv_charge_cmd);

	LOGOMATIC("\n--- BCU_STATUS_2 ---\n");

	LOGOMATIC("\n--- Errors ---\n");
	LOGOMATIC("OVERTEMP: %d\n", state_data->BCU_S2_OVERTEMP_ERROR);
	LOGOMATIC("OVERVOLT: %d\n", state_data->BCU_S2_OVERVOLT_ERROR);
	LOGOMATIC("UNDERVOLT: %d\n", state_data->BCU_S2_UNDERVOLT_ERROR);
	LOGOMATIC("OVERCURR: %d\n", state_data->BCU_S2_OVERCURR_ERROR);
	LOGOMATIC("UNDERCURR: %d\n", state_data->BCU_S2_UNDERCURR_ERROR);

	LOGOMATIC("\n--- Warnings ---\n");
	LOGOMATIC("UNDER20V: %d\n", state_data->BCU_S2_UNDER20v_WARNING);
	LOGOMATIC("UNDER12V: %d\n", state_data->BCU_S2_UNDER12v_WARNING);
	LOGOMATIC("UNDERVOLT SDC: %d\n", state_data->BCU_S2_UNDERVOLTSDC_WARNING);

	LOGOMATIC("\n--- State Bits ---\n");
	LOGOMATIC("SOFTWARE LATCH: %d\n", state_data->SOFTWARE_LATCH);
	LOGOMATIC("PRECHARGE TS ACTIVE: %d\n", state_data->CCU_PRECHARGE_SET_TS_ACTIVE);

	LOGOMATIC("====================================\n\n");

	state_data->request_print_statedata = false;
}
