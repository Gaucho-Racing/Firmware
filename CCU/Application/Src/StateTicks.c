#include "StateTicks.h"

#include <string.h>

#include "CANdler.h"
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"
#include "bitManipulations.h"

static uint32_t millis_since_boot;
static uint32_t last_PRECHARGE_request_millis;

void CCU_State_Tick(CCU_StateData *state_data)
{
	millis_since_boot = MillisecondsSinceBoot();
	switch (state_data->state) {
			// if given an error, switch state to IDLE; warnings will remain placeholders until better understood
			// General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			STATE_IDLE(state_data);
			LOGOMATIC("CCU Current State: Idle\n");
			break;

		case CCU_STATE_CHARGING:
			STATE_CHARGING(state_data);
			LOGOMATIC("CCU Current State: Charging\n");
			break;

		default:
			state_data->state = CCU_STATE_IDLE;
			TripSoftwareLatch(state_data);
			LOGOMATIC("Invalid State Detected, Transitioning to IDLE\n");
			break;
	};
}

void STATE_IDLE(CCU_StateData *state_data)
{
	if (millis_since_boot - last_PRECHARGE_request_millis > PRECHARGE_SET_MSG_PERIOD_MILLIS) {
		SendPrechargeStatus(false);
	}

	ACU_Warnings(state_data);
	if (CriticalError(state_data)) {
		TripSoftwareLatch(state_data);
		LOGOMATIC("Critical Error Occured!\n");
	}
	else if (state_data->recv_charge_cmd) {
		SendPrechargeStatus(true);
		LOGOMATIC("Set PRECHARGE TS ACTIVE = 1\n");
		state_data->recv_charge_cmd = false;

		state_data->state = CCU_STATE_CHARGING;

		LOGOMATIC("CCU Current State: %d\n", state_data->state);
	}
}

void STATE_CHARGING(CCU_StateData *state_data)
{
	ACU_Warnings(state_data);
	if (state_data->recv_stop_cmd) {
		state_data->recv_stop_cmd = false;
		LOGOMATIC("Received STOP command!\n");
		state_data->state = CCU_STATE_IDLE;
		return;
	}

	if (CriticalError(state_data)) {
		TripSoftwareLatch(state_data);
		state_data->state = CCU_STATE_IDLE;

		LOGOMATIC("Critical Error Occured; State Set to IDLE \n");
		return;
	}

	// Checks if IR+/- are in done position
	if (state_data->IR_MINUS && state_data->IR_PLUS) {
		state_data->state = CCU_STATE_IDLE;
		LOGOMATIC("CHARGING is complete, returning to IDLE state");
		return;
	}

	if (state_data->recv_charge_cmd) {
		SendPrechargeStatus(true);
		LOGOMATIC("Set PRECHARGE TS ACTIVE = 1\n");
		state_data->recv_charge_cmd = false;
		return;
	}
}
