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
#define BSE_MAX 2000	    // 1187 max , 1187/4096*3.3 = 0.96
#define BSE_DEADZONE 500    // 688
#define MAX_APPS_IMPLAUSIBLE_TIME_MS 100
#define MAX_BUZZER_TIME_MS 1000
#define BSE_MIN 720

#define REGEN_STRENGTH 2.0f	      // define ratio of regen braking percent to brake pressure percent
#define REGEN_MIN_SPEED_MPH 3.106856f // MPH

#define MAX_CURRENT_AMPS 250.0f	       // TODO: Change as appropriate
#define MAX_REVERSE_CURRENT_AMPS 20.0f // TODO: Change as appropriate

#define MAX_PRECHARGE_TIME 8000 // in ms

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (5000)

// Rate Limiting Macros
/**
 * @brief Macro to rate limit an action to 100 Hz (every 10 ms)
 *
 * Usage:
 * static uint32_t last_action_time = 0;
 * if (RATE_LIMIT_100_HZ(MillisecondsSinceBoot(), last_action_time)) {
 *     // Perform action
 *     last_action_time = MillisecondsSinceBoot();
 * }
 *
 * @param x Current time in milliseconds (e.g., from MillisecondsSinceBoot())
 * @param y Last time the action was performed in milliseconds
 * @return true if at least 10 ms have passed since last action, false otherwise
 */
#define RATE_LIMIT_100_HZ(x, y) (x - y > 10)

/**
 * @brief Macro to rate limit an action to 10 Hz (every 100 ms)
 *
 * Usage:
 * static uint32_t last_action_time = 0;
 * if (RATE_LIMIT_10_HZ(MillisecondsSinceBoot(), last_action_time)) {
 *     // Perform action
 *     last_action_time = MillisecondsSinceBoot();
 * }
 *
 * @param x Current time in milliseconds (e.g., from MillisecondsSinceBoot())
 * @param y Last time the action was performed in milliseconds
 * @return true if at least 100 ms have passed since last action, false otherwise
 */
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
void Send_VCP_APPS(const ECU_StateData *stateData, uint16_t apps1_raw, uint16_t apps2_raw);

#endif
