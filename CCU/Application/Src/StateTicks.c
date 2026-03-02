#include "StateTicks.h"

#include <string.h>

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "bitManipulations.h"

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

void CCU_State_Tick(CCU_StateData *state_data)
{

	LOGOMATIC("CCU Current State: %d\n", state_data->state);

	// FIXME:
	switch (state_data->state) { // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
				     // General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			// TODO: Create IDLE func elsewhere & Call state IDLE function
			STATE_IDLE(state_data);
			break;

		case CCU_STATE_CHARGING:
			// TODO: Create Charging func elsewhere & Call charging func
			STATE_CHARGING(state_data);
			break;

		default:
			state_data->state = CCU_STATE_IDLE;
			break;
	};
}

// TODO: Implement State functionality

void STATE_IDLE(CCU_StateData *state_data)
{
	bool anyErrors = 0;
	if (CriticalError(state_data)) {
		anyErrors = 1;
		setSoftwareLatch(0, state_data);
		LOGOMATIC("Critical Error Occured; State set to IDLE \n");
	};

	if (!anyErrors && state_data->Button_Status) {

		state_data->state = CCU_STATE_CHARGING;
		state_data->BCU_PRECHARGE_SET_TS_ACTIVE = 1;
		SendPrechargeStatus(state_data);
		SendDebugReport("No_Errors");

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}

void STATE_CHARGING(CCU_StateData *state_data)
{

	if (CriticalError(state_data)) {

		setSoftwareLatch(0, state_data);

		state_data->BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		SendPrechargeStatus(state_data);

		state_data->state = CCU_STATE_IDLE;

		LOGOMATIC("Critical Error Occured; State Set to IDLE \n");
	}

	if (!(state_data->Button_Status)) {
		state_data->state = CCU_STATE_IDLE;
		state_data->BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		SendPrechargeStatus(state_data);
		SendDebugReport("No_Error");

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}
