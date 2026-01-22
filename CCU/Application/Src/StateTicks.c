#include <string.h>
#include "StateTicks.h"
#include "CCUStateData.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "Unused.h"
#include "Logomatic.h"

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.



void CCU_State_Tick(CCU_StateData *state_data, CCU_STATE state)
{

	LOGOMATIC("CCU Current State: %d\n");

	// FIXME:
	switch (state) { // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
			 // General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			// TODO: Create IDLE func elsewhere & Call state IDLE function
			STATE_IDLE(state_data, state);
			break;

		case CCU_STATE_CHARGING:
			// TODO: Create Charging func elsewhere & Call charging func
			STATE_CHARGING(state_data, state);
			break;

		default:
			state = CCU_STATE_IDLE;
			break;
	};
}

// TODO: Implement State functionality

void STATE_IDLE(CCU_StateData *state_data, CCU_STATE state)
{
	UNUSED(state_data);
	state_data->ACU_S2_OVERTEMP_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 40);
	state_data->ACU_S2_OVERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 41);
	state_data->ACU_S2_UNDERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 42);
	state_data->ACU_S2_OVERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 43);
	state_data->ACU_S2_UNDERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 44);

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (!anyErrors && state_data->Button_Status) {
		state = CCU_STATE_CHARGING;
	}
}

void STATE_CHARGING(CCU_StateData *state_data, CCU_STATE state)
{
	UNUSED(state_data);
	state_data->ACU_S2_OVERTEMP_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 40);
	state_data->ACU_S2_OVERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 41);
	state_data->ACU_S2_UNDERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 42);
	state_data->ACU_S2_OVERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 43);
	state_data->ACU_S2_UNDERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 44);

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (anyErrors && !(state_data->Button_Status)) {
		state = CCU_STATE_IDLE;
	}
}
