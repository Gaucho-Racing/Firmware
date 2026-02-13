#include <stdbool.h>
#include <stdint.h>

#include "StateData.h"

#ifndef _STATE_UTILS_H_
#define _STATE_UTILS_H_

/// @brief Get the current time in milliseconds since system start
/// @return Current time in milliseconds
uint32_t MillisecondsSinceBoot(void);

// Constants
#define BRAKE_F_MIN 0	    // TODO: need to be determined FIXME: Rename better
#define BRAKE_F_MAX 4095    // TODO: need to be determined FIXME: Rename better
#define BRAKE_R_MIN 0	    // TODO: need to be determined FIXME: Rename better
#define BRAKE_R_MAX 4095    // TODO: need to be determined FIXME: Rename better
#define THROTTLE_MIN_1 0    // TODO: need to be determined
#define THROTTLE_MAX_1 4095 // TODO: need to be determined
#define THROTTLE_MIN_2 0    // TODO: need to be determined
#define THROTTLE_MAX_2 4095 // TODO: need to be determined
#define BSE_DEADZONE 0.05f
#define APPS_PROPORTION 2.0f // TODO: Need to be experimentally determined
#define APPS_OFFSET 250.0f   // TODO: Need to be experimentally determined

#define REGEN_STRENGTH 2.0f	      // define ratio of regen braking percent to brake pressure percent
#define REGEN_MIN_SPEED_MPH 3.106856f // MPH

#define MAX_CURRENT_AMPS 42.0f	       // TODO: Change as appropriate
#define MAX_REVERSE_CURRENT_AMPS 20.0f // TODO: Change as appropriate

// Checks stateData for critical errors
bool CriticalError(volatile const ECU_StateData *stateData);
bool bmsFailure(volatile const ECU_StateData *stateData);
bool imdFailure(volatile const ECU_StateData *stateData);
bool CommunicationError(volatile const ECU_StateData *stateData);
bool APPS_BSE_Violation(volatile const ECU_StateData *stateData);
bool PressingBrake(volatile const ECU_StateData *stateData);
float CalcBrakePercent(volatile const ECU_StateData *stateData);
float CalcPedalTravel(volatile const ECU_StateData *stateData);
bool vehicle_is_moving(volatile const ECU_StateData *stateData);

#endif
