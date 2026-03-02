
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

#ifndef STATE_UTILS_H
#define STATE_UTILS_H

void setSoftwareLatch(bool close, CCU_StateData *state_data);
bool CriticalError(const CCU_StateData *state_data);

#endif
