#include <string.h>

#include "CCUStateData.h"

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

CCU_STATE state = CCU_STATE_IDLE;
CCU_StateData state_data = {0};

void CCU_State_Tick(void)
{

	LOGOMATIC("CCY Current State: %d\n");

	// FIXME:
	switch (state) { // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
			// General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			// TODO: Create IDLE func elsewhere & Call state IDLE functio

		case CCU_STATE_CHARGING:
			// TODO: Create Charging func elsewhere & Call charging func
	};
}

// TODO: Implement State functionality

void STATE_IDLE(CCU_StateData *state_data) {}

void STATE_CHARGING(CCU_StateData *state_data) {}
