#include <string.h>

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "Unused.h"
#include "bitManipulations.h"

static void CCU_PSUEDO_STATE_TICK(CCU_StateData *state_data)
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

int main(void)
{

	// #########
	// No Errors + No Button Pressed (0)
	// #########
	LOGOMATIC("State Tick Started \n");
	CCU_StateData state_dataTest = {0};
	LOGOMATIC("No Errors Occurs=, button is not pressed");

	state_dataTest.state = CCU_STATE_IDLE;
	state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;

	CCU_PSUEDO_STATE_TICK(&state_dataTest);

	// #########
	// No Errors + Button Pressed (1)
	// #########
	LOGOMATIC("No Errors Occurs, button is pressed");
	CCU_StateData state_dataTest = {0};

	state_dataTest.state = CCU_STATE_IDLE;
	state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
	state_dataTest.Button_Status = 1;

	CCU_PSUEDO_STATE_TICK(&state_dataTest);

	if (state_dataTest.state != CCU_STATE_CHARGING) {
		LOGOMATIC("CCU STATE did not switch to CHARGING");
		return 1;
	}

	if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 1) {
		LOGOMATIC("PRECHARGE Message did not send correct message");
		return 1;
	}

	// #########
	// 1 Error + No Button Pressed (2)
	// #########
	LOGOMATIC("1 Errors Occurs, button is not pressed");
	CCU_StateData state_dataTest = {0};

	state_dataTest.state = CCU_STATE_IDLE;
	state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;

	CCU_PSUEDO_STATE_TICK(&state_dataTest);

	if (state_dataTest.state != CCU_STATE_IDLE) {
		LOGOMATIC("CCU STATE did not stay IDLE");
		return 2;
	}

	if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
		LOGOMATIC("PRECHARGE Message did not send correct message");
		return 2;
	}

	// #########
	// 1 Error + Button Pressed (3)
	// #########

	// #########
	// Some Errors + Button Pressed (4)
	// #########

	// #########
	// Every Error + Button Pressed (5)
	// #########

	// #########
	// No Error + Button Pressed ON then OFF (6)
	// #########
}
