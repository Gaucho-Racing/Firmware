#include "CCUStateData.h"

#include <string.h>

// FIXME: Currently defining check to switch CCU state from idle to charging and etc.

ccu_state_t *state = {0};

CCU_StateData state_data = {0};



void StateTransition(CCU_StateData* state_data, ccu_state_t* state){
	

	// FIXME: 
	switch(state){ // if given an error, switch state to IDLE; warnings will remain placeholders until better understood
		// General checks for State Transition, if any error detected, transition back to IDLE state

		case OVER_TEMP_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func
		
		case OVER_VOLT_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func

		case UNDER_VOLT_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func

		case OVER_CURR_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func

		case UNDER_CURR_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func

		case UNDER_20v_WARNING:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func

		case UNDER_12v_WARNING:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func


		case UNDER_SDC_WARNING:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func


		case PRECHARGE_ERROR:
		// Define later with Bit Manipulation (check BITMANIPULATION_LIB) aka getBits func






	};


}