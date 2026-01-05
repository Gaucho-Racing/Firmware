#include <stdbool.h>
#include <stdint.h>

#include "StateData.h"

#ifndef _STATE_UTILS_H_
#define _STATE_UTILS_H_

// Constants
#define BRAKE_F_MIN 0	    // TODO: need to be determined
#define BRAKE_F_MAX 4095    // TODO: need to be determined
#define BRAKE_R_MIN 0	    // TODO: need to be determined
#define BRAKE_R_MAX 4095    // TODO: need to be determined
#define THROTTLE_MIN_1 0    // TODO: need to be determined
#define THROTTLE_MAX_1 4095 // TODO: need to be determined
#define THROTTLE_MIN_2 0    // TODO: need to be determined
#define THROTTLE_MAX_2 4095 // TODO: need to be determined
#define BSE_DEADZONE 0.05f
#define APPS_PROPORTION 2.0f // TODO: Need to be experimentally determined
#define APPS_OFFSET 250.0f   // TODO: Need to be experimentally determined

// Checks stateData for critical errors
bool CriticalError(const ECU_StateData *stateData);
bool CommunicationError(const ECU_StateData *stateData);
bool APPS_BSE_Violation(const ECU_StateData *stateData);
bool PressingBrake(const ECU_StateData *stateData);
float getBrakePercent(const ECU_StateData *stateData);
float getPedalTravel(const ECU_StateData *stateData);

#endif
