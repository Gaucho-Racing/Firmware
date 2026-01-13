#include "CCUStateData.h"

#include <string.h>

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

ccu_state_t state;

CCU_StateData state_data = {0};



void StateTransition(CCU_StateData* state_data, ccu_state_t* state){
	

	// FIXME: 
	switch(*state){ // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
		// General checks for State Transition, if any error detected, transition back to IDLE state

		case CCU_STATE_IDLE:
			//TODO: Create IDLE func elsewhere & Call state IDLE functio
		
		case CCU_STATE_CHARGING:
			//TODO: Create Charging func elsewhere & Call charging func




	};


}