#include <stdbool.h>
#include <stdint.h>

#include "StateData.h"

#ifndef _STATE_UTILS_H_
#define _STATE_UTILS_H_

/// @brief Get the current time in milliseconds since system start
/// @return Current time in milliseconds
uint32_t MillisecondsSinceBoot(void);

// Constants
#define MAX_APPS_IMPLAUSIBLE_TIME_MS 100
#define MAX_BUZZER_TIME_MS 1000

#define REGEN_MIN_SPEED_MPH 3.106856f // MPH

#define MAX_CURRENT_AMPS 300.0f	       // Determined by Ryan
#define MAX_REVERSE_CURRENT_AMPS 20.0f // TODO: Change as appropriate

#define ECU_STATUS_MSG_PERIOD_MILLIS (1000)
#define TRACTIVE_SYSTEM_MAX_PERMITTED_DISCHARGE_TIME_MILLIS (5000)

// Rate Limiting Macros
/**
 * @brief Macro to rate limit an action to 100 Hz (every 10 ms)
 *
 * Usage:
 * static uint32_t last_action_time = 0;
 * if (RATE_LIMIT_100_HZ(MillisecondsSinceBoot(), last_action_time)) {
 *     last_action_time = MillisecondsSinceBoot();
 *     // Perform action
 * }
 *
 * @param x Current time in milliseconds (e.g., from MillisecondsSinceBoot())
 * @param y Last time the action was performed in milliseconds
 * @return true if at least 10 ms have passed since last action, false otherwise
 */
#define RATE_LIMIT_100_HZ(x, y) ((x) - (y) > 10)

/**
 * @brief Macro to rate limit an action to 10 Hz (every 100 ms)
 *
 * Usage:
 * static uint32_t last_action_time = 0;
 * if (RATE_LIMIT_10_HZ(MillisecondsSinceBoot(), last_action_time)) {
 *     last_action_time = MillisecondsSinceBoot();
 *     // Perform action
 * }
 *
 * @param x Current time in milliseconds (e.g., from MillisecondsSinceBoot())
 * @param y Last time the action was performed in milliseconds
 * @return true if at least 100 ms have passed since last action, false otherwise
 */
#define RATE_LIMIT_10_HZ(x, y) ((x) - (y) > 100)

typedef enum {
	SDC_OK,
	SDC_ONGOING_FAILURE,
	SDC_LATCHED_FAILURE
} SDC_Level;

// Checks stateData for critical errors
bool CriticalError(volatile const ECU_StateData *stateData);
SDC_Level bmsLevel(volatile const ECU_StateData *stateData);
SDC_Level imdLevel(volatile const ECU_StateData *stateData);
SDC_Level bspdLevel(volatile const ECU_StateData *stateData);
bool bmsFailure(volatile const ECU_StateData *stateData);
bool imdFailure(volatile const ECU_StateData *stateData);
bool bspdFailure(volatile const ECU_StateData *stateData);
bool APPS_BSE_Violation(volatile const ECU_StateData *stateData);
bool PressingBrake(volatile const ECU_StateData *stateData);
float CalcBrakePressure(volatile const ECU_StateData *stateData);
float CalcAccPedalTravel(volatile const ECU_StateData *stateData);
bool APPS_Plausible(volatile const ECU_StateData *stateData);
/* Disable inverter for both DTI and Custom */
void disable_inverter(void);
void Send_VCP_APPS(const ECU_StateData *stateData, uint16_t apps1_raw, uint16_t apps2_raw);

#endif
