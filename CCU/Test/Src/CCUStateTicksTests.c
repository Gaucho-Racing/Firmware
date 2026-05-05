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

		// Test 1: in Idle, receive charge command with no errors
		LOGOMATIC("In Idle, receive charge command with no errors\n");
		state_dataTest.recv_charge_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Should be no errors and move to Charging\n");
			return 1;
		}
		LOGOMATIC("\n\n\n");

		// Test 2: in Charging, receive stop charge command with no errors
		LOGOMATIC("In Charging, receive stop charge command with no errors\n");
		state_dataTest.recv_stop_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Should be no errors and move to Idle\n");
			return 2;
		}
		LOGOMATIC("\n\n\n");

		// Test 3: in Idle, receive charge command, 1 error
		LOGOMATIC("In Idle, receive charge command, 1 error\n");
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.recv_charge_cmd = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be an overtemp error and stay in state Idle\n");
			return 3;
		}
		LOGOMATIC("\n\n\n");

		// Test 4: in Idle, receive charge command, 2 errors
		LOGOMATIC("in Idle, receive charge command, 2 errors");
		state_dataTest.recv_charge_cmd = 1;
		state_dataTest.BCU_S2_UNDERCURR_ERROR = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be a critical error and stay in state Idle\n");
			return 4;
		}
		LOGOMATIC("\n\n\n");

		// Test 5: in Charging, no start or stop command, but an error
		LOGOMATIC("In Charging, no start or stop command, but an error\n");
		state_dataTest.recv_charge_cmd = 1;
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 0;
		state_dataTest.BCU_S2_UNDERCURR_ERROR = 0;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		state_dataTest.BCU_S2_UNDERCURR_ERROR = 1;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("There should be a critical error and move to Idle\n");
			return 5;
		}
		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("----Recv Charge Cmd, Charging Complete, Return to IDLE---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.recv_charge_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Did not move to charging\n");
			return 6;
		}

		state_dataTest.BCU_S2_IR_MINUS = true;
		state_dataTest.BCU_S2_IR_PLUS = true;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not move to IDLE after charging complete\n");
			return 6;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 6;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("----Recv Charge Cmd, IR PRECHARGE, IR CHARGE COMPLETE, Return to IDLE---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.recv_charge_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Did not move to charging\n");
			return 7;
		}

		state_dataTest.BCU_S2_IR_MINUS = true;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Should not have moved to IDLE\n");
			return 7;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 7;
		}

		state_dataTest.BCU_S2_IR_PLUS = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not move to IDLE after charging complete\n");
			return 7;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 7;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("----Recv Charge Cmd, Charging Complete, Return to IDLE---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.recv_charge_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Did not move to charging\n");
			return 8;
		}

		state_dataTest.BCU_S2_IR_MINUS = true;
		state_dataTest.BCU_S2_IR_PLUS = true;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not move to IDLE after charging complete\n");
			return 8;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 8;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("----Recv Charge Cmd, IR IMPOSSIBILITY ERROR, Return to IDLE, Trip Latch---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.recv_charge_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Did not move to charging\n");
			return 9;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 9;
		}

		state_dataTest.BCU_S2_IR_PLUS = true;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not move to IDLE after encountering CritError\n");
			return 9;
		}

		if (state_dataTest.SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software latch should have tripped\n");
			return 9;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("----Recv Charge Cmd, Recv Stop Cmd, Return to IDLE---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.recv_charge_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("Did not move to charging\n");
			return 10;
		}

		state_dataTest.recv_stop_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not move to IDLE after receiving stop cmd\n");
			return 10;
		}
		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		//
		// #########
		LOGOMATIC("---- Recv Stop Cmd in IDLE, STAY to IDLE---- \n");
		CCU_StateData state_dataTest = {0, .SOFTWARE_LATCH = 1};

		state_dataTest.state = CCU_STATE_IDLE;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not stay IDLE\n");
			return 11;
		}

		state_dataTest.recv_stop_cmd = true;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("Did not stay IDLE\n");
			return 11;
		}

		if (state_dataTest.SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software latch should not have tripped\n");
			return 11;
		}
		LOGOMATIC("\n\n\n");
	}
	LOGOMATIC("All TESTS PASSED \n");
}
