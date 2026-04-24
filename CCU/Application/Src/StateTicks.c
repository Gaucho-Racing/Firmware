#include "StateTicks.h"

#include <string.h>

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "bitManipulations.h"

void CCU_State_Tick(CCU_StateData *state_data)
{

	switch (state_data->state) { // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
				     // General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			STATE_IDLE(state_data);
			break;

		case CCU_STATE_CHARGING:
			STATE_CHARGING(state_data);
			break;

		default:
			state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;
			state_data->state = CCU_STATE_IDLE;
			SendPrechargeStatus(state_data);
			setSoftwareLatch(state_data);
			break;
	};
}

void STATE_IDLE(CCU_StateData *state_data)
{

	BCU_Warnings(state_data);
	bool anyErrors = CriticalError(state_data);
	if (anyErrors) {
		setSoftwareLatch(state_data);
		LOGOMATIC("Critical Error Occured!\n");
	}

	else if (!anyErrors && state_data->recv_charge_cmd) {

		state_data->state = CCU_STATE_CHARGING;
		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = true;
		SendPrechargeStatus(state_data);

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}

void STATE_CHARGING(CCU_StateData *state_data)
{

	BCU_Warnings(state_data);
	if (CriticalError(state_data)) {

		setSoftwareLatch(state_data);

		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;
		SendPrechargeStatus(state_data);

		state_data->state = CCU_STATE_IDLE;

		LOGOMATIC("Critical Error Occured; State Set to IDLE \n");
	}

	else if (!(state_data->recv_charge_cmd)) {
		state_data->state = CCU_STATE_IDLE;
		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;
		SendPrechargeStatus(state_data);

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}
