#include <string.h>
#include "CCUStateData.h"
#include "StateMachine.h"
#include "bitManipulations.h"

void CCU_State_Tick(CCU_StateData *state_data, CCU_STATE state);
void STATE_IDLE(CCU_StateData *state_data, CCU_STATE state);
void STATE_CHARGING(CCU_StateData *state_data, CCU_STATE state);
