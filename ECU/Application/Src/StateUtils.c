#include "StateUtils.h"

#include <math.h>
#include <stdbool.h>

#include "Logomatic.h"
#include "StateData.h"
#include "main.h"

uint32_t millis(void)
{
	// For some reason, GetTickFreq returns period in ms instead of frequency LMAO
	// See https://community.st.com/t5/stm32-mcus-embedded-software/name-amp-description-of-hal-gettickfreq-misleading/td-p/242457
	return HAL_GetTick() * HAL_GetTickFreq();
}

// use estop_sense to detect close(?)
void setSoftwareLatch(bool close)
{
	UNUSED(close);
	// TODO Implement functionality
	LOGOMATIC("Setting software latch to %d\n", close);


	if (close && !LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already written to
	    {
		LL_GPIO_SetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	    }
	    else if (!close && LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
	    {
		LL_GPIO_ResetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
		}

}

bool CriticalError(volatile const ECU_StateData *stateData)
{
	bool problem = false;
	problem |= stateData->max_cell_temp > 60;
	problem |= stateData->ts_voltage > 600;
	problem |= APPS_BSE_Violation(stateData);
	problem |= !stateData->bcu_software_latch;
	problem |= stateData->ir_plus && !stateData->ir_minus;
	problem |= !stateData->ir_plus && (stateData->ecu_state == GR_PRECHARGE_COMPLETE || stateData->ecu_state == GR_DRIVE_ACTIVE);
	problem |= !stateData->ir_minus && (stateData->ecu_state == GR_PRECHARGE_ENGAGED || stateData->ecu_state == GR_PRECHARGE_COMPLETE || stateData->ecu_state == GR_DRIVE_ACTIVE);
	return problem;
}

bool CommunicationError(volatile const ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO: im
	LOGOMATIC("Communication Error"); plement COMMS errors

	return false;
}

bool APPS_BSE_Violation(volatile const ECU_StateData *stateData)
{
	// Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
	return fabs(stateData->APPS2_Signal - stateData->APPS1_Signal * APPS_PROPORTION - APPS_OFFSET) > stateData->APPS2_Signal * 0.1f ||
	       (PressingBrake(stateData) && CalcPedalTravel(stateData) >= 0.25f);
}

bool PressingBrake(volatile const ECU_StateData *stateData)
{
	uint16_t brakeRangeF = BRAKE_F_MAX - BRAKE_F_MIN;
	uint16_t brakeRangeR = BRAKE_R_MAX - BRAKE_R_MIN;
	bool brakeFpress = stateData->Brake_F_Signal - BRAKE_F_MIN > BSE_DEADZONE * brakeRangeF;
	bool brakeRpress = stateData->Brake_R_Signal - BRAKE_R_MIN > BSE_DEADZONE * brakeRangeR;
	return brakeFpress || brakeRpress;
	// Ideally TCM receives values of 0 after this is no longer called xD.
}

float CalcBrakePercent(volatile const ECU_StateData *stateData) // THIS IS NOT ACTUALLY BRAKE TRAVEL,
								// PRESSURE SENSORS CAPTURE BRAKE TRAVEL
{
	float total_brake_range = BRAKE_F_MAX - BRAKE_F_MIN + BRAKE_R_MAX - BRAKE_R_MIN;
	float total_brake_value = stateData->Brake_F_Signal + stateData->Brake_R_Signal - BRAKE_R_MIN - BRAKE_F_MIN;
	return total_brake_value / total_brake_range;
}

float CalcPedalTravel(volatile const ECU_StateData *stateData)
{
	float total_signal_range = THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2;
	float total_signal_value = stateData->APPS1_Signal + stateData->APPS2_Signal - THROTTLE_MIN_2 - THROTTLE_MIN_1;
	return total_signal_value / total_signal_range;
}

bool vehicle_is_moving(volatile const ECU_StateData *stateData){
	const float tolerance = 0.1; // In MPH
	return stateData->vehicle_speed > tolerance;
}
