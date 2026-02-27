#include <string.h>

#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "Unused.h"
#include "main.h"
#include "stm32g4xx_hal.h"

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
	{
		// #########
		// No Errors + No Button Pressed (0)
		// #########
		LOGOMATIC("----State Tick Started---- \n");
		CCU_StateData state_dataTest = {0};
		LOGOMATIC("No Errors Occurs, button is not pressed\n");

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);
		LOGOMATIC("\n\n\n");
	}
	{
		// #########
		// No Errors + Button Pressed (1)
		// #########
		LOGOMATIC("----No Errors Occurs, button is pressed----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("CCU STATE did not switch to CHARGING\n");
			return 1;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 1) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 1;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software Latch tripped when it shouldn't\n");
			return 1;
		}
		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// 1 Error + No Button Pressed (2)
		// #########
		LOGOMATIC("----1 Errors Occurs, button is not pressed----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;
		state_dataTest.BCU_S2_OVERCURR_ERROR = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 2;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 2;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software Latch was not tripped and set to low\n");
			return 2;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// 1 Error + Button Pressed (3)
		// #########
		LOGOMATIC("----1 Errors Occurs, button pressed----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 3;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 3;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software Latch was not tripped and set to low\n");
			return 3;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// Some Errors + Button Pressed (4)
		// #########

		LOGOMATIC("----Some Errors Occur, button pressed----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.BCU_S2_OVERVOLT_ERROR = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 4;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 4;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software Latch was not tripped and set to low\n");
			return 4;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// Every Error + Button Pressed (5)
		// #########

		LOGOMATIC("----Every Error Occurs, button pressed----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;
		state_dataTest.BCU_S2_OVERVOLT_ERROR = 1;
		state_dataTest.BCU_S2_UNDERCURR_ERROR = 1;
		state_dataTest.BCU_S2_UNDERVOLT_ERROR = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 5;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 5;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software Latch was not tripped and set to low\n");
			return 5;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// No Error + Button Pressed ON then OFF (6)
		// #########

		LOGOMATIC("----No Errors Occur, button is pressed ON then OFF----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("CCU STATE did not stay CHARGING\n");
			return 6;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 1) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 6;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software Latch was tripped and set to low\n");
		}

		state_dataTest.Button_Status = 0;
		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 6;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 6;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software Latch was tripped and set to low\n");
			return 6;
		}

		LOGOMATIC("\n\n\n");
	}
	{
		// ######### FIXME: Double check valid test
		// No Error, is Charging + Then Error (7)
		// #########
		LOGOMATIC("----In Charging, then error occurs----\n");
		CCU_StateData state_dataTest = {0};

		state_dataTest.state = CCU_STATE_IDLE;
		state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE = 0;
		state_dataTest.Button_Status = 1;
		state_dataTest.BCU_S2_SOFTWARE_LATCH = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_CHARGING) {
			LOGOMATIC("CCU STATE did not stay CHARGING\n");
			return 7;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 1) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 7;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 1) {
			LOGOMATIC("Software Latch was tripped and set to low\n");
		}

		state_dataTest.BCU_S2_OVERTEMP_ERROR = 1;

		CCU_PSUEDO_STATE_TICK(&state_dataTest);

		if (state_dataTest.state != CCU_STATE_IDLE) {
			LOGOMATIC("CCU STATE did not stay IDLE\n");
			return 7;
		}

		if (state_dataTest.BCU_PRECHARGE_SET_TS_ACTIVE != 0) {
			LOGOMATIC("PRECHARGE Message did not send correct message\n");
			return 7;
		}

		if (state_dataTest.BCU_S2_SOFTWARE_LATCH != 0) {
			LOGOMATIC("Software Latch was not tripped and set to low\n");
			return 7;
		}
		LOGOMATIC("\n\n\n");
	}
	LOGOMATIC("----ALL CURRENT TEST CASES PASSED\n----");
	return 0;
}
