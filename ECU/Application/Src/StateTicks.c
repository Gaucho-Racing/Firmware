#include "StateTicks.h"

#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
#include "StateUtils.h"
#include "Unused.h"

/**
 * @brief The ECU state data lump.
 *
 * Lumped state data for the ECU state machine passed to each state function on
 * tick.
 *
 * @remark Intentionally not a globally accessible variable
 */
ECU_StateData stateLump = {0};

void ECU_State_Tick(void)
{
	LOGOMATIC("ECU Current State: %d\n", stateLump.ecuStatus1.ecu_status);

	switch (stateLump.ecuStatus1.ecu_status) {
		case GR_GLV_OFF:
			ECU_GLV_Off(&stateLump);
			break;
		case GR_GLV_ON:
			ECU_GLV_On(&stateLump);
			break;
		case GR_PRECHARGE_ENGAGED:
			ECU_Precharge_Engaged(&stateLump);
			break;
		case GR_PRECHARGE_COMPLETE:
			ECU_Precharge_Complete(&stateLump);
			break;
		case GR_DRIVE_ACTIVE:
			ECU_Drive_Active(&stateLump);
			break;
		case GR_TS_DISCHARGE_OFF:
			ECU_Tractive_System_Discharge(&stateLump);
			break;
		default:
			LOGOMATIC("ECU Current State Unknown: %d\n",
				  stateLump.ecuStatus1.ecu_status);
			LOGOMATIC("ECU: Resetting to GLV On\n");
			stateLump.ecuStatus1.ecu_status = GR_GLV_ON;
			break;
	}
}

/*

TODO: implement state functionality when loading INTO the state, not just
transitioning state

*/

void ECU_GLV_Off(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	// ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	UNUSED(stateData);
	/*
	if(stateData->TractiveSystemVoltage >= 60){ // should never happen but
	has to be accounted for stateData->currentState = GR_TS_DISCHARGE_OFF;
		emit an error
		break;
	}
	*/

	// TODO Implement functionality
	if (stateData->ts_active_button_engaged) {
		stateData->ecuStatus1.ecu_status = GR_PRECHARGE_ENGAGED;
	}
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	UNUSED(stateData);
	if (stateData->ecuStatus2.ts_voltage > 60) {
		// Go to TS discharge
		stateData->ecuStatus1.ecu_status = GR_TS_DISCHARGE_OFF;
		// Emit an error
		return;
	}
	// TODO Implement functionality
	/*if(not TS Active || Communication Error (CAN)){
		stateData->currentState = GR_TS_DISCHARGE_OFF
		break;
	}*/
	/*if(1 Isolation relay close && second isolation relay close){ --> CAN!
		stateData->currentState = GR_PRECHARGE_COMPLETE
	}*/
}

void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	/*
		On but idle

		If Tractive System (TS) active/Critical Error --> Tractive
	    System Discharge If Brake & RTD (Ready to Drive) --> Drive Active
	*/
	/*
	if (TS pressed or critical error) {
		stateData->currentState = GR_TS_DISCHARGE_OFF
		emit error
		break;
	}
	*/
	/*
	if(PressingBrake(stateData) && stateData->RTD){
		stateData->currentState = GR_DRIVE_ACTIVE;
	}
	*/

	// Pseudocode
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	/*
		If APPS/BSE Violation --> Don't drive until resolved (no state
		change) If Tractive System (TS) active/Critical Error -->
	   Tractive System Discharge
			--> pressed again
		If RTD (Ready to Drive) --> Precharge Complete
	*/

	// Pseudocode
	/*
		if (!TSActive || criticalError(stateData)) {
			stateData->currentState = GR_TS_DISCHARGE_OFF
			emit an error
			break
		}
		if (!RTD) {
			stateData->currentState = GR_PRECHARGE_COMPLETE
			emit a warning if not moving
			break
		}
		and then we drive the car
		 - calcPedalTravel func :p
		 - make tuna-ble function
	*/
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality of state itself
	/*
		Discharge the tractive system to below 60 volts
		If TS voltage < 60 --> stateData->GLV_ON
	*/
	if (stateData->ecuStatus2.ts_voltage < 60) {
		stateData->ecuStatus1.ecu_status = GR_GLV_ON;
	}
	/*
		If TS fails to discharge over time then stay and emit a warning,
	   see #129
	*/
}
