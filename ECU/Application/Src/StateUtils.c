#include "StateUtils.h"

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "CANutils.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Plan_C.h"
#include "StateData.h"
#include "Unused.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_gpio.h"
#include "vcp.h"

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

SDC_Level bmsLevel(volatile const ECU_StateData *stateData)
{
	// TODO: DYNAMIC LOGIC HERE
	if (stateData->bms_sense < stateData->bms_min_thresh) {
		return SDC_ONGOING_FAILURE;
	} else if (stateData->bms_sense > stateData->bms_max_thresh) {
		return SDC_LATCHED_FAILURE;
	}

	return SDC_OK;
}

SDC_Level imdLevel(volatile const ECU_StateData *stateData)
{
	// TODO: DYNAMIC LOGIC HERE
	if (stateData->imd_sense < stateData->imd_min_thresh) {
		return SDC_ONGOING_FAILURE;
	} else if (stateData->imd_sense > stateData->imd_max_thresh) {
		return SDC_LATCHED_FAILURE;
	}

	return SDC_OK;
}

SDC_Level bspdLevel(volatile const ECU_StateData *stateData)
{
	if (stateData->bspd_sense < stateData->bspd_min_thresh) {
		return SDC_ONGOING_FAILURE;
	} else if (stateData->bspd_sense > stateData->bspd_max_thresh) {
		return SDC_LATCHED_FAILURE;
	}

	return SDC_OK;
}

bool bmsFailure(volatile const ECU_StateData *stateData)
{
	SDC_Level level = bmsLevel(stateData);
	return level == SDC_ONGOING_FAILURE || level == SDC_LATCHED_FAILURE;
}

bool imdFailure(volatile const ECU_StateData *stateData)
{
	if (MillisecondsSinceBoot() < IMD_SENSE_FAILURE_DELAY_MS) {
		// Ignore IMD sense failures to allow sensor to stabilize after startup
		// See https://discord.com/channels/756738476887638107/1254635893306953769/1498219532224041021 for more details
		return false;
	}

	SDC_Level level = imdLevel(stateData);
	return level == SDC_ONGOING_FAILURE || level == SDC_LATCHED_FAILURE;
}

bool bspdFailure(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return false;
#endif
	SDC_Level level = bspdLevel(stateData);
	return level == SDC_ONGOING_FAILURE || level == SDC_LATCHED_FAILURE;
}

bool APPS_BSE_Violation(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return false;
#endif

	return PressingBrake(stateData) && CalcAccPedalTravel(stateData) > (0.25f + stateData->apps_deadzone);
}

bool PressingBrake(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	if (stateData->ecu_state < GR_DRIVE_ACTIVE) {
		return true;
	} else {
		return false;
	}
#endif

	return stateData->bse_signal > stateData->brake_bse_min;
}

float CalcBrakePressure(volatile const ECU_StateData *stateData)
{
#ifdef PLAN_C
	return 0;
#endif

	return stateData->bse_signal / 4096.0f * 5000.0f;
}

// TODO: reconsider deadzone
float CalcAccPedalTravel(volatile const ECU_StateData *stateData)
{
	float appspos1 = (stateData->APPS1_Signal - stateData->apps_1_min) / (float)(stateData->apps_1_max - stateData->apps_1_min);
	float appspos2 = (stateData->APPS2_Signal - stateData->apps_2_min) / (float)(stateData->apps_2_max - stateData->apps_2_min);

	float travel = fminf(fmaxf((appspos1 + appspos2) / 2.0f, 0.0f), 1.0f);
	return travel > stateData->apps_deadzone ? (travel - stateData->apps_deadzone) / (1.0f - stateData->apps_deadzone) : 0.0f;
}

// APPS implausibility check (within 10% travel)
bool APPS_Plausible(volatile const ECU_StateData *stateData)
{
	float appspos1 = (stateData->APPS1_Signal - stateData->apps_1_min) / (float)(stateData->apps_1_max - stateData->apps_1_min);
	float appspos2 = (stateData->APPS2_Signal - stateData->apps_2_min) / (float)(stateData->apps_2_max - stateData->apps_2_min);

	float error = fabsf(appspos1 - appspos2);

	return error < 0.1f;
}

void disable_inverter(void)
{
	GRCAN_INV_CMD_MSG inverter_msg = {.drive_enable = 0, .field_weakening = 0, .rpm_limit = 0, .set_ac_current = 32768, .set_dc_current = 32768};
	ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_GR_Inv, GRCAN_INV_CMD, &inverter_msg, sizeof(inverter_msg));
	ECU_CAN_Send_DTI(DTI_CONTROL_12_CAN_ID, &inverter_msg.drive_enable, 1);
}

void Send_VCP_APPS(const ECU_StateData *stateData, uint16_t apps1_raw, uint16_t apps2_raw)
{
#define SIZE 64
	static char buf[SIZE];
	snprintf(buf, SIZE, "%" PRIu32 " A1 %d A2 %d A1R %d A2R %d\n", MillisecondsSinceBoot(), stateData->APPS1_Signal, stateData->APPS2_Signal, apps1_raw, apps2_raw);
	VCP_Send(buf, strlen(buf));
}
