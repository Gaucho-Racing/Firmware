#include <string.h>

#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateTicks.h"
#include "StateUtilsTests.h"
#include "Unused.h"
#include "main.h"
#include "stm32g4xx_ll_gpio.h"

static void CCU_PSUEDO_STATE_TICK(CCU_StateData *state_data)
{

	LOGOMATIC("CCU Current State: %d\n", state_data->state);

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
	{
		// #########
		//
		// #########
		LOGOMATIC("----State Tick Started---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		// //state_dataTest\.CCU_PRECHARGE_SET_TS_ACTIVE = 0;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		LOGOMATIC("\n\n\n");
	}
}
