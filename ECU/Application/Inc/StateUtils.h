#include <stdbool.h>
#include <stdint.h>

#include "StateData.h"

#ifndef _STATE_UTILS_H_
#define _STATE_UTILS_H_

/// @brief Get the current time in milliseconds since system start
/// @return Current time in milliseconds
uint32_t MillisecondsSinceBoot(void);

// Constants
#define BRAKE_F_MIN 700	    // TODO: need to be determined FIXME: Rename better
#define BRAKE_F_MAX 4095    // TODO: need to be determined FIXME: Rename better
#define BRAKE_R_MIN 0	    // TODO: need to be determined FIXME: Rename better
#define BRAKE_R_MAX 4095    // TODO: need to be determined FIXME: Rename better
#define THROTTLE_MIN_1 2375 // TODO: need to be determined
#define THROTTLE_MAX_1 1897 // TODO: need to be determined
#define THROTTLE_MIN_2 2430 // TODO: need to be determined
#define THROTTLE_MAX_2 1926 // TODO: need to be determined
#define BSE_MAX 2000 // 1187 max , 1187/4096*3.3 = 0.96
#define BSE_DEADZONE 500 // 688
#define MAX_BSE_FAILURE_TIME 100
#define MAX_APPS_FAILURE_TIME 100
#define MAX_BUZZER_TIME_MS 1000
#define APPS_PROPORTION 2.0f // TODO: Need to be experimentally determined
#define APPS_OFFSET 250.0f   // TODO: Need to be experimentally determined
#define BSE_MIN 720

#define REGEN_STRENGTH 2.0f	      // define ratio of regen braking percent to brake pressure percent
#define REGEN_MIN_SPEED_MPH 3.106856f // MPH

#define MAX_CURRENT_AMPS 250.0f	       // TODO: Change as appropriate
#define MAX_REVERSE_CURRENT_AMPS 20.0f // TODO: Change as appropriate

#define MAX_PRECHARGE_TIME 8000 // in ms

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (5000)

// Rate Limiting Macros
#define RATE_LIMIT_100_HZ(x, y) (x - y > 10)
#define RATE_LIMIT_10_HZ(x, y) (x - y > 100)

// Checks stateData for critical errors
bool CriticalError(volatile const ECU_StateData *stateData);
bool bmsFailure(volatile const ECU_StateData *stateData);
bool imdFailure(volatile const ECU_StateData *stateData);
bool bspdFailure(volatile const ECU_StateData *stateData);
bool APPS_BSE_Violation(volatile const ECU_StateData *stateData);
bool PressingBrake(volatile const ECU_StateData *stateData);
float CalcBrakePercent(volatile const ECU_StateData *stateData);
float CalcAccPedalTravel(volatile const ECU_StateData *stateData);
bool APPS_Plausible(volatile const ECU_StateData *stateData);
bool vehicle_is_moving(volatile const ECU_StateData *stateData);
/* Disable inverter for both DTI and Custom */
void disable_inverter(void);

#endif
