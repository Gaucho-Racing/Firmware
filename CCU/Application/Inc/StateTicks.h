#include <string.h>

#include "CCUStateData.h"
#include "StateMachine.h"
#include "bitManipulations.h"

#ifndef STATE_TICKS_H
#define STATE_TICKS_H
bool anyErrors = 0;
void CCU_State_Tick(CCU_StateData *state_data);
void STATE_IDLE(CCU_StateData *state_data);
void STATE_CHARGING(CCU_StateData *state_data);
#endif
