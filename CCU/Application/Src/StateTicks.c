#include "StateTicks.h"

#include <string.h>

#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "Unused.h"
#include "bitManipulations.h"

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

void CCU_State_Tick(CCU_StateData *state_data)
{

	LOGOMATIC("CCU Current State: %d\n", state_data->state); // Logo I think not working

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

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERVOLT_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (!anyErrors && state_data->Button_Status) {
		state_data->state = CCU_STATE_CHARGING;
	}
}

void STATE_CHARGING(CCU_StateData *state_data)
{

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERVOLT_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (anyErrors || !(state_data->Button_Status)) {
		state_data->state = CCU_STATE_IDLE;
	}
}
