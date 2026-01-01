#include "StateTicks.h"

#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"
#include "Errors.h"
#include "Unused.h"
#include "StateUtils.h"

/**
 * @brief The ECU state data lump.
 *
 * Lumped state data for the ECU state machine passed to each state function on
 * tick.
 *
 * @remark Intentionally not a globally accessible variable
 */
ECU_StateData stateLump = {0};
volatile uint8_t numberOfBadMessages = 0;

void ECU_State_Tick(void)
{
	LOGOMATIC("ECU_State_Tick: Current State: %d\n",
		  stateLump.currentState);

	switch (stateLump.currentState) {
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
			LOGOMATIC("ECU_State_Tick: Unknown State %d\n",
				  stateLump.currentState);
			LOGOMATIC("ECU_State_Tick: Resetting to GR_GLV_ON\n");
			stateLump.currentState = GR_GLV_ON;
			break;
	}
}

void ECU_GLV_Off(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	// ERROR --> GLV_OFF should never be reached 
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	if(stateData->TractiveSystemVoltage >= 60){
		stateData->currentState = GR_TS_DISCHARGE_OFF;
	}

	// TODO Implement functionality
	/*if(TS Active is pressed){
	* 	stateData->currentState = GR_PRECHARGE_ENGAGED
	}*/
	/*
	if(TS voltage >= 60) {
		Go to TS discharge
		Emit an error
	}
	*/
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	/*if(TS Active is pressed || Communication Error (CAN)){
		stateData->currentState = GR_TS_DISCHARGE_OFF
	}*/
	/*if(1 Isolation relay close && second isolation relay close){
		stateData->currentState = GR_PRECHARGE_COMPLETE
	}*/
	/*
	if(TS voltage >= 60) {
		Go to TS discharge
		Emit an error
	}
	*/
}

void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	/*
		On but idle

		If Tractive System (TS) active/Critical Error --> Tractive System Discharge
		If Brake & RTD (Ready to Drive) --> Drive Active
	*/
	if(pressingBrake() && stateData->RTD){
		stateData->currentState = GR_DRIVE_ACTIVE;
	}

	// Pseudocode
	/*
		if (stateData->brake >= 5% && RTD pressed) {
			stateData->currentState = GR_DRIVE_ACTIVE
		}
		if (TS pressed or critical error) {
			stateData->currentState = GR_TS_DISCHARGE_OFF
		}
	*/
	

}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	/*
		If RTD (Ready to Drive) --> Precharge Complete
		If APPS/BSE Violation --> Don't drive until resolved (no state change)
		If Tractive System (TS) active/Critical Error --> Tractive System Discharge
			--> pressed again
	*/

	// Pseudocode
	/*
		if (TSActive pressed again OR criticalError(stateData)) {
			stateData->currentState = GR_TS_DISCHARGE_OFF
			emit an error
		}
		if (RTD pressed again) {
			stateData->currentState = GR_PRECHARGE_COMPLETE
			emit a warning if not moving
	*/
	
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	// TODO Implement functionality
	/*
		Discharge the tractive system to below 60 volts
		If TS voltage < 60 --> GLV_ON
	*/
	/*
		If TS fails to discharge over time then stay and emit a warning, see #129
	*/
}
