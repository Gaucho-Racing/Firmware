#include "StateData.h"
#include "StateMachine.h"

#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

/**
 *
 */
void ECU_State_Tick(void);

/**
 *
 */
void ECU_GLV_Off(ECU_StateData *stateData);

/**
 *
 */
void ECU_GLV_On(ECU_StateData *stateData);

/**
 *
 */
void ECU_Precharge_Engaged(ECU_StateData *stateData);

/**
 *
 */
void ECU_Precharge_Complete(ECU_StateData *stateData);

/**
 *
 */
void ECU_Drive_Active(ECU_StateData *stateData);

/**
 *
 */
void ECU_Tractive_System_Discharge(ECU_StateData *stateData);

#endif
