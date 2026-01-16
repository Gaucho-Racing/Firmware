#include "StateUtils.h"

#include <math.h>
#include <stdbool.h>

#include "StateData.h"
#include "Unused.h"

// use estop_sense to detect close(?)
void setSoftwareLatch(bool close)
{
	UNUSED(close);
	// TODO Implement functionality
	// LOGOMATIC("Setting software latch to %d\n", close);
	/*
	    if (close && !HAL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already
	   written too
	    {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
	    }
	    else if (!close && HAL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin))
	    {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
	    }
<<<<<<< HEAD
	*/
	/*
		if (close && !LL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already
	   written too
	    {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
	    }
	    else if (!close && HAL_GPIO_IsInputPinSet(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin))
	    {
		HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port,
	   SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
	*/
}

bool CriticalError(volatile const ECU_StateData *stateData)
{
	if (stateData->max_cell_temp > 60) {
		return true;
	}
	if (stateData->ts_voltage > 600) {
		return true;
	}
	if (stateData->bse_apps_violation) {
		return true;
	}
	return false;
}

bool CommunicationError(volatile const ECU_StateData *stateData)
{
	UNUSED(stateData);
	// TODO: implement COMMS errors
	return false;
}

bool APPS_BSE_Violation(volatile const ECU_StateData *stateData)
{
	// Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the
	// same time
	return fabs(stateData->APPS2_Signal - stateData->APPS1_Signal * APPS_PROPORTION - APPS_OFFSET) > stateData->APPS2_Signal * 0.1f ||
	       (PressingBrake(stateData) && CalcPedalTravel(stateData) >= 0.25f);
}

bool PressingBrake(volatile const ECU_StateData *stateData)
{
	return (stateData->Brake_F_Signal - BRAKE_F_MIN > BSE_DEADZONE * (BRAKE_F_MAX - BRAKE_F_MIN)) && (stateData->Brake_R_Signal - BRAKE_R_MIN > BSE_DEADZONE * (BRAKE_R_MAX - BRAKE_R_MIN));
	// Ideally TCM receives values of 0 after this is no longer called xD.
}

float CalcBrakePercent(volatile const ECU_StateData *stateData) // THIS IS NOT ACTUALLY BRAKE TRAVEL,
								// PRESSURE SENSORS CAPTURE BRAKE TRAVEL
{
	return (float)(stateData->Brake_F_Signal + stateData->Brake_R_Signal - BRAKE_R_MIN - BRAKE_F_MIN) / (BRAKE_F_MAX - BRAKE_F_MIN + BRAKE_R_MAX - BRAKE_R_MIN);
}

float CalcPedalTravel(volatile const ECU_StateData *stateData)
{
	return (float)(stateData->APPS1_Signal + stateData->APPS2_Signal - THROTTLE_MIN_2 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2);
}
