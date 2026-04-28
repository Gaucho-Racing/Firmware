#include "StateTicks.h"

#include <string.h>

#include "CANDler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "bitManipulations.h"

static uint32_t mills_since_boot;
static uint32_t last_PRECHARGE_request_millis;

void CCU_State_Tick(CCU_StateData *state_data)
{
	mills_since_boot = MillsSinceBoot();
	switch (state_data->state) {
			// if given an error, switch state to IDLE; warnings will remain placeholders until better understood
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
	if (CriticalError(state_data)) {
		setSoftwareLatch(state_data);
		LOGOMATIC("Critical Error Occured!\n");
	}

	else if (state_data->recv_charge_cmd) {

		state_data->state = CCU_STATE_CHARGING;
		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = true;

		state_data->precharge_step = PRECHARGE_STEP_WAIT_IR_MINUS; // ensures that now the switch statement will actually be executed/triggered after sending precharge status message

		if (mills_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
			SendPrechargeStatus(state_data); // IR- should be set to 1 at this point, IR+ may become 1 if charging complete
			last_PRECHARGE_request_millis = mills_since_boot;
		}

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}

void STATE_CHARGING(CCU_StateData *state_data)
{
	BCU_Warnings(state_data);
	if (CriticalError(state_data)) {

		setSoftwareLatch(state_data);

		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;

		if (mills_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
			SendPrechargeStatus(state_data);
			last_PRECHARGE_request_millis = mills_since_boot;
		}

		state_data->state = CCU_STATE_IDLE;

		LOGOMATIC("Critical Error Occured; State Set to IDLE \n");
		return;
	}

	else if (!(state_data->recv_charge_cmd)) {

		state_data->state = CCU_STATE_IDLE;
		state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;

		if (mills_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
			SendPrechargeStatus(state_data);
			last_PRECHARGE_request_millis = mills_since_boot;
		}

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
		return;
	}
	// now actual IR checks are being done in the charging state
	switch (state_data->precharge_step) {
		case PRECHARGE_STEP_WAIT_IR_MINUS:
			if (state_data->BCU_S2_PRECHARGE_STATE) {
				LOGOMATIC("IR- confirmed closed, waiting for IR+\n");
				state_data->precharge_step = PRECHARGE_STEP_WAIT_IR_PLUS; // go to next phase of checking for IR+
			}
			break;

		case PRECHARGE_STEP_WAIT_IR_PLUS:
			if (!IR_Sanity_Check(state_data)) {
				LOGOMATIC("IR Sanity Check Failed! Transitioning back to IDLE\n");
				state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;
				state_data->state = CCU_STATE_IDLE;

				if (mills_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
					SendPrechargeStatus(state_data); // IR- should be set to 1 at this point, IR+ may become 1 if charging complete
					last_PRECHARGE_request_millis = mills_since_boot;
				}
				return;
			}
			if (state_data->BCU_S2_IR_STATE) {
				LOGOMATIC("IR+ confirmed closed, precharge complete\n");
				state_data->precharge_step = PRECHARGE_STEP_COMPLETE;
			}
			break;

		case PRECHARGE_STEP_COMPLETE:
			if (!IR_Sanity_Check(state_data)) {
				LOGOMATIC("IR Sanity Check Failed in steady state! Transitioning back to IDLE\n");
				state_data->CCU_PRECHARGE_SET_TS_ACTIVE = false;
				state_data->state = CCU_STATE_IDLE;

				if (mills_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
					SendPrechargeStatus(state_data); // IR- should be set to 1 at this point, IR+ may become 1 if charging complete
					last_PRECHARGE_request_millis = mills_since_boot;
				}

				return;
			}
			break;

		default:
			state_data->precharge_step = PRECHARGE_STEP_WAIT_IR_MINUS;
			break;
	}
}
