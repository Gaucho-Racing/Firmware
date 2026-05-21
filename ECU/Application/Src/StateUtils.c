#include "StateUtils.h"

#include <math.h>
#include <stdbool.h>

#include "CANutils.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
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
	// problem |= !stateData->acu_software_latch; // No
	problem |= stateData->ir_plus && !stateData->ir_minus;
	problem |= !stateData->ir_plus && (stateData->ecu_state == GR_PRECHARGE_COMPLETE || stateData->ecu_state == GR_DRIVE_ACTIVE);  // ensured pre charge is complete via ir+ latch
	problem |= !stateData->ir_minus && (stateData->ecu_state == GR_PRECHARGE_COMPLETE || stateData->ecu_state == GR_DRIVE_ACTIVE); // ensures precharge has begun with ir- latch
	problem |= imdFailure(stateData);
	problem |= bmsFailure(stateData);
	problem |= bspdFailure(stateData);
	if (problem) {
		LOGOMATIC("Critical Error Detected in State %d | %f C | %f V | ACU SW %d | IR+ %d | IR- %d | IMD %d | BMS %d | BSPD %d\n", stateData->ecu_state, stateData->max_cell_temp_c,
			  stateData->ts_voltage, stateData->acu_software_latch, stateData->ir_plus, stateData->ir_minus, imdFailure(stateData), bmsFailure(stateData), bspdFailure(stateData));
		// LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	}
	return problem;
}

bool bmsFailure(volatile const ECU_StateData *stateData)
{
	return stateData->ams_sense < 0.5f || stateData->ams_sense > 1.6f; // 0.5 to 1.6 is valid
}

bool imdFailure(volatile const ECU_StateData *stateData)
{
	if (MillisecondsSinceBoot() < IMD_SENSE_FAILURE_DELAY_MS) {
		// Ignore IMD sense failures to allow sensor to stabilize after startup
		// See https://discord.com/channels/756738476887638107/1254635893306953769/1498219532224041021 for more details
		return false;
	}

	return stateData->imd_sense < 0.5f || stateData->imd_sense > 1.6f; // 0.5 to 1.6 is valid
}

bool bspdFailure(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return false;
#endif

	return stateData->bspd_sense < 0.6f || stateData->bspd_sense > 1.35f; // possible values are 0.3, 1.2, 1.6
}

bool APPS_BSE_Violation(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return false;
#endif

	// Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
	return PressingBrake(stateData) && CalcAccPedalTravel(stateData) >= 0.25f;
}

// TODO: reconsider deadzones
bool PressingBrake(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	if (stateData->ecu_state < GR_DRIVE_ACTIVE) {
		return true;
	} else {
		return false;
	}
#endif

	// uint16_t brakeRangeF = BRAKE_F_MAX - BRAKE_F_MIN;
	// uint16_t brakeRangeR = BRAKE_R_MAX - BRAKE_R_MIN;
	// bool brakeFpress = stateData->Brake_F_Signal - BRAKE_F_MIN > BSE_DEADZONE * brakeRangeF;
	// bool brakeRpress = stateData->Brake_R_Signal - BRAKE_R_MIN > BSE_DEADZONE * brakeRangeR;
	// return brakeFpress || brakeRpress;
	// FIXME: DELETE THE FOLLOWING CONTROL BLOCK FOR BRAKE TESTING
	if (stateData->Brake_F_Signal > (BRAKE_F_MIN) || stateData->bse_signal > (BSE_MIN)) {
		return true;
	}
	return false;
	// Ideally TCM receives values of 0 after this is no longer called xD.
}

float CalcBrakePercent(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return 0;
#endif

	return stateData->bse_signal / 4096.0f;
}

// TODO: reconsider deadzone
float CalcAccPedalTravel(volatile const ECU_StateData *stateData)
{
	// float appspos1 = (stateData->APPS1_Signal - THROTTLE_MIN_1) / (float)(THROTTLE_MAX_1 - THROTTLE_MIN_1);
	// float appspos2 = (stateData->APPS2_Signal - THROTTLE_MIN_2) / (float)(THROTTLE_MAX_2 - THROTTLE_MIN_2);
	// float travel = fminf(fmaxf((2.0f - appspos1 + appspos2) / 2.0f, 0.0f), 1.0f);
	float travel1 = (stateData->APPS1_Signal - THROTTLE_MIN_1) * (1.0f - 0.0f) / (THROTTLE_MAX_1 - THROTTLE_MIN_1) + 0.0f;
	float travel2 = (stateData->APPS2_Signal - THROTTLE_MIN_2) * (1.0f - 0.0f) / (THROTTLE_MAX_2 - THROTTLE_MIN_2) + 0.0f;
	float travel = (travel1 + travel2) / 2.0f;
	return travel > 0.05f ? (travel - 0.05f) / 0.95f : 0.0f;
}

// APPS implausibility check (within 10% travel)
bool APPS_Plausible(volatile const ECU_StateData *stateData)
{
	float travel1 = (stateData->APPS1_Signal - THROTTLE_MIN_1) * (1.0f - 0.0f) / (THROTTLE_MAX_1 - THROTTLE_MIN_1) + 0.0f;
	float travel2 = (stateData->APPS2_Signal - THROTTLE_MIN_2) * (1.0f - 0.0f) / (THROTTLE_MAX_2 - THROTTLE_MIN_2) + 0.0f;
	return fabsf(travel1 - travel2) < 0.1f;
}

bool vehicle_is_moving(volatile const ECU_StateData *stateData)
{
	const float tolerance = 0.1f; // In MPH
	return stateData->vehicle_speed_mph > tolerance;
}

void SendEcuBonusInfo(const ECU_StateData *stateData)
{
	// All analog data
	GRCAN_ECU_ANALOG_DATA_MSG analogData = {.bspd_signal = stateData->bspd_signal,
						.bse_signal = stateData->bse_signal,
						.apps_1_signal = stateData->APPS1_Signal,
						.apps_2_signal = stateData->APPS2_Signal,
						.brakeline_f_signal = stateData->Brake_F_Signal,
						.brakeline_r_signal = stateData->Brake_R_Signal,
						.steering_angle_signal = stateData->steering_angle_signal,
						.aux_signal = stateData->aux_signal};
	ECU_CAN_Send(GRCAN_BUS_DATA, GRCAN_TCM, GRCAN_ECU_ANALOG_DATA, &analogData, sizeof(analogData));

	// RTT ping data
	// TODO Setup using data from Pinging.c per Andrey request
}

void disable_inverter(void)
{
	GRCAN_INVERTER_COMMAND_MSG inverter_msg = {.drive_enable = 0, .field_weakening = 0, .rpm_limit = 0, .set_ac_current = 0, .set_dc_current = 0};
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inverter, GRCAN_INVERTER_COMMAND, &inverter_msg, sizeof(inverter_msg));
	ECU_CAN_Send_DTI(DTI_CONTROL_12_CAN_ID, &inverter_msg.drive_enable, 1);
}
