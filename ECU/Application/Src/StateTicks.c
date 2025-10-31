#include "StateTicks.h"
#include "Logomatic.h"
#include "StateData.h"
#include "StateMachine.h"

#include "main.h"

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
			Error_Handler();
			break;
	}
}

void ECU_GLV_Off(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}

void ECU_GLV_On(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}

void ECU_Precharge_Engaged(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}

void ECU_Precharge_Complete(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}

void ECU_Drive_Active(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}

void ECU_Tractive_System_Discharge(ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO Implement functionality
}
