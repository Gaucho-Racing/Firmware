#include <stdbool.h>
#include "StateUtils.h"
#include "StateData.h"

void setSoftwareLatch(bool close)
{
    // LOGOMATIC("Setting software latch to %d\n", close);
/*
    if (close && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already written too
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (!close && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
        */
}

bool CriticalError(const ECU_StateData *stateData)
{
    if (stateData->MaxCellTemp > 60){
        return true;
    }
    if (stateData->TractiveSystemVoltage > 600) {
        return true;
    }
    return false;
}

bool CommunicationError(const ECU_StateData *stateData)
{
    // TODO: implement COMMS errors
    return false;
}

bool APPS_BSE_Violation(const ECU_StateData *stateData)
{
    //Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
    return fabs(stateData->APPS2_SIGNAL - stateData->APPS1_SIGNAL * APPS_PROPORTION - APPS_OFFSET) > stateData->APPS2_SIGNAL * 0.1f
            || (PressingBrake(stateData) && getPedalTravel(stateData) >= 0.25f);
}

bool PressingBrake(const ECU_StateData *stateData)
{
    return (stateData->BRAKE_F_SIGNAL - BRAKE_F_MIN > BSE_DEADZONE * (BRAKE_F_MAX - BRAKE_F_MIN))
    && (stateData->BRAKE_R_SIGNAL - BRAKE_R_MIN > BSE_DEADZONE * (BRAKE_R_MAX - BRAKE_R_MIN));
    //Ideally TCM receives values of 0 after this is no longer called xD.
}

float getBrakePercent(const ECU_StateData *stateData) // THIS IS NOT ACTUALLY BRAKE TRAVEL, PRESSURE SENSORS CAPTURE BRAKE TRAVEL
{
    return (float)(stateData->BRAKE_F_SIGNAL + stateData->BRAKE_R_SIGNAL - BRAKE_R_MIN - BRAKE_F_MIN) / (BRAKE_F_MAX - BRAKE_F_MIN + BRAKE_R_MAX - BRAKE_R_MIN);
}

float getPedalTravel(const ECU_StateData *stateData)
{
    return (float)(stateData->APPS1_SIGNAL + stateData->APPS2_SIGNAL - THROTTLE_MIN_2 - THROTTLE_MIN_1) / (THROTTLE_MAX_1 + THROTTLE_MAX_2 - THROTTLE_MIN_1 - THROTTLE_MIN_2);
}
