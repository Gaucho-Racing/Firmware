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
volatile ECU_StateData stateLump = {0};

#define ECU_STATUS_MSG_PERIOD (100)
#define TRACTIVE_SYSTEM_MAX_DISCHARGE_TIME (10000) // TODO: determine an appropriate wait time

void ECU_State_Tick(void)
{
	if (stateLump.lastECUStatusMsgTick >= ECU_STATUS_MSG_PERIOD) {
		LOGOMATIC("ECU Current State: %d\n", stateLump.ecu_state);
		stateLump.lastECUStatusMsgTick = 0;
	} else {
		stateLump.lastECUStatusMsgTick++;
	}

	switch (stateLump.ecu_state) {
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
		case GR_TS_DISCHARGE:
			ECU_Tractive_System_Discharge(&stateLump);
			break;
		default:
			LOGOMATIC("ECU Current State Unknown: %d\n", stateLump.ecu_state);
			LOGOMATIC("ECU: Resetting to GLV On\n");
			stateLump.ecu_state = GR_GLV_ON;
			break;
	}
}

/*

TODO: implement state functionality when loading INTO the state, not just
transitioning state

*/

void ECU_GLV_Off(volatile ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
	// ERROR --> GLV_OFF should never be reached
}

void ECU_GLV_On(volatile ECU_StateData *stateData)
{
	if (stateData->ts_voltage >= 60) { // should never happen but has to be accounted for
		ECU_Tractive_System_Discharge_Start(stateData);
		// emit an error
		return;
	}

	// TODO Implement functionality
	if (stateData->ts_active_button_engaged) {
		stateData->ecu_state = GR_PRECHARGE_ENGAGED;
	}
}

void ECU_Precharge_Engaged(volatile ECU_StateData *stateData)
{
	if (stateData->ts_voltage >= 60) {
		ECU_Tractive_System_Discharge_Start(stateData);
		// emit an error
		return;
	}
	// TODO Implement functionality
	if (!stateData->ts_active_button_engaged || CommunicationError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		return;
	}
	/*if(1 Isolation relay close && second isolation relay close){ --> CAN!
		stateData->currentState = GR_PRECHARGE_COMPLETE
	}*/
}

void ECU_Precharge_Complete(volatile ECU_StateData *stateData)
{
	// TODO Implement functionality
	/*
		On but idle

		If Tractive System (TS) active/Critical Error --> Tractive
	    System Discharge If Brake & RTD (Ready to Drive) --> Drive Active
	*/
	/*
	if (TS pressed or critical error) {
		stateData->currentState = GR_TS_DISCHARGE
		emit error
		return;
	}
	*/
	/*
	if(PressingBrake(stateData) && stateData->RTD){
		stateData->currentState = GR_DRIVE_ACTIVE;
	}
	*/

	// Pseudocode
	if (stateData->ts_active_button_engaged || CriticalError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		// emit an error
		return;
	}

	if (PressingBrake(stateData) && stateData->rtd_button_engaged) {
		stateData->ecu_state = GR_DRIVE_ACTIVE;
	}
}

void ECU_Drive_Active(volatile ECU_StateData *stateData)
{
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
			stateData->currentState = GR_TS_DISCHARGE
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

	if (!stateData->ts_active_button_engaged || CriticalError(stateData)) {
		ECU_Tractive_System_Discharge_Start(stateData);
		return;
	}

	if (!stateData->rtd_button_engaged) {
		stateData->ecu_state = GR_PRECHARGE_COMPLETE;
		// emit a warning if not moving
		return;
	}
}

void ECU_Tractive_System_Discharge_Start(volatile ECU_StateData *stateData)
{
	stateData->ecu_state = GR_TS_DISCHARGE;
	LOGOMATIC("tell the BCU to discharge TS");
	stateData->dischargeStartMillis = 0;
}

void ECU_Tractive_System_Discharge(volatile ECU_StateData *stateData)
{
	// TODO Implement functionality of state itself
	/*
		Discharge the tractive system to below 60 volts
		If TS voltage < 60 --> stateData->GLV_ON
	*/
	// TODO: Discharge TC through CAN
	LOGOMATIC("CAN: please discharge the Tractive System");
	if (stateData->ts_voltage < 60) {
		stateData->ecu_state = GR_GLV_ON;
		stateData->dischargeStartMillis = 0;
		return;
	}
	/*
		If TS fails to discharge over time then stay and emit a warning,
	   see #129
	*/
	// TODO: Determine the maximum time to wait for TC to discharge.
	if (stateData->dischargeStartMillis > TRACTIVE_SYSTEM_MAX_DISCHARGE_TIME) {
		// TODO: Research appropriate ways to buffer warning messages.
		LOGOMATIC("Tractive System fails to discharge in time.");
	}

	if (stateData->dischargeStartMillis < INT32_MAX) {
		stateData->dischargeStartMillis++;
	}
}
