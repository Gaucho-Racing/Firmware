#include <string.h>

#include "CCUStateData.h"
#include "bitManipulations.h"

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

CCU_STATE state = CCU_STATE_IDLE;

CCU_StateData state_data = {0};

void CCU_State_Tick(void)
{

	LOGOMATIC("CCU Current State: %d\n");

	// FIXME:
	switch (state) { // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
			 // General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			// TODO: Create IDLE func elsewhere & Call state IDLE function
			STATE_IDLE(&state_data);
			break;

		case CCU_STATE_CHARGING:
			// TODO: Create Charging func elsewhere & Call charging func
			STATE_CHARGING(&state_data);
			break;

		default:
			state = CCU_STATE_IDLE;
			break;
	};
}

// TODO: Implement State functionality

void STATE_IDLE(CCU_StateData *state_data)
{
	state_data->ACU_S2_OVERTEMP_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 40);
	state_data->ACU_S2_OVERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 41);
	state_data->ACU_S2_UNDERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 42);
	state_data->ACU_S2_OVERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 43);
	state_data->ACU_S2_UNDERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 44);

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (!anyErrors) {
		state = CCU_STATE_CHARGING;
	}
}

void STATE_CHARGING(CCU_StateData *state_data) {

	state_data->ACU_S2_OVERTEMP_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 40);
	state_data->ACU_S2_OVERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 41);
	state_data->ACU_S2_UNDERVOLT_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 42);
	state_data->ACU_S2_OVERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 43);
	state_data->ACU_S2_UNDERCURR_ERROR = CHECK_BIT(state_data->ACU_S2_ERROR_BITS, 44);

	bool anyErrors =
	    state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_OVERTEMP_ERROR || state_data->ACU_S2_UNDERVOLT_ERROR || state_data->ACU_S2_OVERCURR_ERROR || state_data->ACU_S2_UNDERCURR_ERROR;

	if (anyErrors) {
		state = CCU_STATE_IDLE;
	}
}
