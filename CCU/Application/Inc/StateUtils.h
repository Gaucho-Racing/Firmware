
#include "CCUStateData.h"
#include "Logomatic.h"
#include "StateMachine.h"
#include "bitManipulations.h"
#include "can.h"
#include "gpio.h"

#ifndef STATE_UTILS_H
#define STATE_UTILS_H

void VCP_Oneliner(const CCU_StateData *state_data);
void TripSoftwareLatch(CCU_StateData *state_data);
bool CriticalError(const CCU_StateData *state_data);
bool ACU_Warnings(const CCU_StateData *state_data);
uint32_t MillisecondsSinceBoot(void);
bool IR_Check(CCU_StateData *state_data);
#endif
