#include "StateUtils.h"

#include <math.h>
#include <stdbool.h>

#include "Logomatic.h"
#include "StateData.h"
#include "Unused.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_gpio.h"

/**
 * @brief Delay after startup to allow IMD sense to stabilize before considering IMD sense failures valid
 *
 * This is necessary because the IMD sense can read as a failure during the initial stabilization period after startup,
 * which could lead to false critical error detections and unnecessary tractive system discharges or red car violations.
 * By ignoring IMD sense failures for the first IMD_SENSE_FAILURE_DELAY_MS milliseconds after boot, we allow the sensor
 * to stabilize and provide accurate readings before it can trigger a failure condition.
 *
 * @note The specific duration should be determined before entering a controlled environment, such as competition, to
 * avoid triggering a red car violation due to a false critical error detection on startup.
 */
#define IMD_SENSE_FAILURE_DELAY_MS 5000

uint32_t MillisecondsSinceBoot(void)
{
	// For some reason, GetTickFreq returns period in milliseconds instead of frequency
	// See https://community.st.com/t5/stm32-mcus-embedded-software/name-amp-description-of-hal-gettickfreq-misleading/td-p/242457
	return HAL_GetTick() * HAL_GetTickFreq();
}

bool CriticalError(volatile const ECU_StateData *stateData)
{
	bool problem = false;
	problem |= stateData->max_cell_temp_c > CRITICAL_MAX_CELL_TEMP_C;
	problem |= stateData->ts_voltage > CRITICAL_TS_VOLTAGE;
	problem |= !stateData->bcu_software_latch; // when latch is OPEN (0), then system shut down
	problem |= stateData->ir_plus && !stateData->ir_minus;
	problem |= !stateData->ir_plus && (stateData->ecu_state == GR_PRECHARGE_COMPLETE || stateData->ecu_state == GR_DRIVE_ACTIVE); // ensured pre charge is complete via ir+ latch
	problem |= !stateData->ir_minus && (stateData->ecu_state == GR_PRECHARGE_ENGAGED || stateData->ecu_state == GR_PRECHARGE_COMPLETE ||
					    stateData->ecu_state == GR_DRIVE_ACTIVE); // ensures precharge has begun with ir- latch
	problem |= imdFailure(stateData);
	problem |= bmsFailure(stateData);
	problem |= bspdFailure(stateData);
	if (problem) {
		// LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	}
	return problem;
}

bool bmsFailure(volatile const ECU_StateData *stateData)
{
	return (stateData->ams_sense >= 2.0f) || (stateData->ams_sense <= 1.0f); // TODO: find better range
}

bool imdFailure(volatile const ECU_StateData *stateData)
{
	if (MillisecondsSinceBoot() < IMD_SENSE_FAILURE_DELAY_MS) {
		// Ignore IMD sense failures to allow sensor to stabilize after startup
		// See https://discord.com/channels/756738476887638107/1254635893306953769/1498219532224041021 for more details
		return false;
	}

	return (stateData->imd_sense >= 2.0f) || (stateData->imd_sense <= 1.0f); // TODO: find better range
}

bool bspdFailure(volatile const ECU_StateData *stateData)
{
	return (stateData->bspd_sense >= 2.0f) || (stateData->bspd_sense <= 1.0f); // TODO: find better range
										   // TODO: shutdown switch stuff
}

bool APPS_BSE_Violation(volatile const ECU_StateData *stateData)
{
	// Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
	return PressingBrake(stateData) && CalcAccPedalTravel(stateData) >= 0.25f;
}

// TODO: reconsider deadzones
bool PressingBrake(volatile const ECU_StateData *stateData)
{
	// uint16_t brakeRangeF = BRAKE_F_MAX - BRAKE_F_MIN;
	// uint16_t brakeRangeR = BRAKE_R_MAX - BRAKE_R_MIN;
	// bool brakeFpress = stateData->Brake_F_Signal - BRAKE_F_MIN > BSE_DEADZONE * brakeRangeF;
	// bool brakeRpress = stateData->Brake_R_Signal - BRAKE_R_MIN > BSE_DEADZONE * brakeRangeR;
	// return brakeFpress || brakeRpress;
	return ((stateData->bse_signal) / 4096.0f * 3.3f) > BSE_DEADZONE;
	// Ideally TCM receives values of 0 after this is no longer called xD.
}

float CalcBrakePercent(volatile const ECU_StateData *stateData)
{
	float total_brake_range = BRAKE_F_MAX - BRAKE_F_MIN + BRAKE_R_MAX - BRAKE_R_MIN;
	float total_brake_value = stateData->Brake_F_Signal + stateData->Brake_R_Signal - BRAKE_R_MIN - BRAKE_F_MIN;
	return total_brake_value / total_brake_range;
}

// TODO: reconsider deadzone
float CalcAccPedalTravel(volatile const ECU_StateData *stateData)
{
	float total_signal_range = THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2;
	float total_signal_value = stateData->APPS1_Signal + stateData->APPS2_Signal - THROTTLE_MIN_2 - THROTTLE_MIN_1;
	float travel = total_signal_value / total_signal_range;
	return travel > 0.05f ? (travel - 0.05f) / 0.95f : 0.0f;
}

// APPS implausibility check (within 10% travel)
bool APPS_Plausible(volatile const ECU_StateData *stateData)
{
	float deviation = (stateData->APPS1_Signal - THROTTLE_MIN_1 - stateData->APPS2_Signal + THROTTLE_MIN_2) * 2.0f / (THROTTLE_MAX_1 - THROTTLE_MIN_1 + THROTTLE_MAX_2 - THROTTLE_MIN_2);
	return deviation < 0.1f && deviation > -0.1f;
}

bool BSE_Implausible(volatile const ECU_StateData *stateData)
{
	// checks for BSE signal failures --> > max failure time (100 ms) then result in apps/bse violation and kill motors
	// T.4.3.3
	if (stateData->bse_signal < BSE_DEADZONE) { // TODO: fix deadzone l8r
		return true;
	}
	return false;
}

bool vehicle_is_moving(volatile const ECU_StateData *stateData)
{
	const float tolerance = 0.1f; // In MPH
	return stateData->vehicle_speed_mph > tolerance;
}
