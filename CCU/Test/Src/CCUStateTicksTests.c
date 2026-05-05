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

		// Test 1: in Idle, receive charge command with no errors
		state_dataTest.recv_charge_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if(state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Should be no errors and move to Charging\n");
			return 1;
		}

		// Test 2: in Charging, receive stop charge command with no errors
		state_dataTest.recv_stop_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if(state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Should be no errors and move to Idle\n");
			return 2;
		}

		// Test 3: in Idle, receive charge command, 1 error
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.recv_charge_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if(state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be an overtemp error and stay in state Idle\n");
			return 3;
		}

		// Test 4: in Idle, receive charge command, 2 errors
		state_dataTest.recv_charge_cmd = 1;
		state_dataTest.BCU_S2_UNDERCURR_ERROR = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if(state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be a critical error and stay in state Idle\n");
			return 4;
		}

		// Test 5: in Charging, no start or stop command, but an error
		state_dataTest.recv_charge_cmd = 1;
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 0;
		state_dataTest.BCU_S2_UNDERCURR_ERROR = 0;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		state_dataTest.BCU_S2_UNDERCURR_ERROR = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if(state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be a critical error and move to Idle\n");
			return 5;
		}
	}
}
