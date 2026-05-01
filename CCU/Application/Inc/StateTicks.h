#include <string.h>

#include "CCUStateData.h"
#include "StateMachine.h"
#include "bitManipulations.h"

#ifndef STATE_TICKS_H
#define STATE_TICKS_H
void CCU_State_Tick(CCU_StateData *state_data);
void STATE_IDLE(CCU_StateData *state_data);
void STATE_CHARGING(CCU_StateData *state_data);

#define PRECHARGE_SET_MSG_PERIOD_MILLIS 50

#endif
