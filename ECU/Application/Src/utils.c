#include <stdbool.h>
#include <stdio.h>

#include "fdcan.h"
#include "stm32g4xx_hal.h"
#include "utils.h"
#include "msgIDs.h"
#include "main.h"
#include "stateMachine.h"
#include "CANdler.h"
#include "driving.h"
#include "math.h"
#include "inverter.h"
#include "driving.h"
#include "adc.h"

uint32_t millis(void)
{
    return HAL_GetTick() * (1000 / TICK_FREQ);
}

//0 indexed LSB first
bool getBit(uint8_t number, uint8_t index)
{
    return getBits(number, index, 1);   // Equivalent to `(number >> index) & 0x1
}

//0 indexed LSB first
uint8_t getBits(uint8_t number, uint8_t index, uint8_t length)
{
    return (number >> index) & ((1 << length) - 1);
}

void setSoftwareLatch(bool close)
{
    // LOGOMATIC("Setting software latch to %d\n", close);

    if (close && !HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin)) // Avoid writing pins that are already written too
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_SET);
    }
    else if (!close && HAL_GPIO_ReadPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin))
    {
        HAL_GPIO_WritePin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin, GPIO_PIN_RESET);
    }
}

bool checkBSEAPPSviolation(uint16_t throttle1, uint16_t throttle2, float pedalTravel, float brake)
{
    //Checks 2 * APPS_1 is within 10% of APPS_2 and break + throttle at the same time
    return fabs(throttle2 - throttle1 * 1.9932988878f - 250.251982816f) > throttle2 * 0.1f || ((brake >= BSE_DEADZONE || pressingBrake()) && pedalTravel >= 0.25f);
}

#ifdef ENABLE_THREE_MOTORS
void validateForwardTorqueRequest(uint16_t* tqr, volatile uint16_t* heatCapacity)
{
    float deltaH =  0.01f * *tqr * *tqr - NOMINAL_CURRENT_FORWARD * NOMINAL_CURRENT_FORWARD;
    *heatCapacity += (*heatCapacity + deltaH  > 0) ? deltaH : -1 * *heatCapacity;
    float proportionOfMax = (1 - ((float)*heatCapacity/MAX_AMK_HEAT_CAP - minAmkHeatCapThrottlePercent) / 0.1f);
    proportionOfMax = (proportionOfMax < 0) ? 0 : proportionOfMax;
    if (*heatCapacity > MAX_AMK_HEAT_CAP * minAmkHeatCapThrottlePercent && *tqr > MAX_CURRENT_FORWARD * proportionOfMax)
    {
        *tqr = MAX_CURRENT_FORWARD * proportionOfMax;
    }
}
#endif

void validateRegenRequest(uint16_t* fwdTqr1, uint16_t* fwdTqr2, uint16_t* rTqr)
{
    float throttlePercent = 1 - (float)(*fwdTqr1 + *fwdTqr2 + *rTqr)/MAX_REVERSE_CURRENT;

    throttlePercent = throttlePercent < 0 ? 0 : throttlePercent;
    throttlePercent = throttlePercent > 1 ? 1 : throttlePercent;
    throttlePercent *= (1 - (float)(globalStatus.MaxCellTemp - MIN_BAT_TEMP_REGEN_THROTTLE) / 5);

    *fwdTqr1 *= throttlePercent;
    *fwdTqr2 *= throttlePercent;
    *rTqr *= throttlePercent;
}

bool pressingBrake(void)
{
    globalStatus.BRAKE_F_SIGNAL = (float)globalAnalog.BRAKE_F_SIGNAL;
    globalStatus.BRAKE_R_SIGNAL = (float)globalAnalog.BRAKE_R_SIGNAL;
    return (globalStatus.BRAKE_F_SIGNAL - BRAKE_F_MIN > BSE_DEADZONE * (BRAKE_F_MAX - BRAKE_F_MIN)) && (globalStatus.BRAKE_R_SIGNAL - BRAKE_R_MIN > BSE_DEADZONE * (BRAKE_R_MAX - BRAKE_R_MIN));
    //Ideally TCM receives values of 0 after this is no longer called xD.
}

float vehicleSpeedMPH(void)
{
    return (globalInverterData.msgOne.erpm * M_TWOPI * WHEEL_RADIUS_IN) / (MOTOR_POLE_PAIRS * GEAR_RATIO * MPH_TO_INCH_PER_MIN_CONV);
}

void sendBseAppsViolationMessage(void)
{
    uint8_t errorMap = 0x1;
    writeMessage(PrimaryBusCAN, MSG_DASH_WARNING_FLAGS, GR_DASH_PANEL, &errorMap, 1);
}

bool ACUError(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t errorBits = acuMsgTwo->Error_Warning_Bits > 0;

    if (errorBits != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "ACUErr%hhX", errorBits);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);  // Not sending '\0'

        char steeringMsg[16];
        snprintf(steeringMsg, 16, "ACU Error -- %hhX", errorBits);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 15);   // Not sending '\0'

        LOGOMATIC("ACU Error -- %hhX", errorBits);

        return 1;
    }

    return acuMsgTwo->SDC_Voltage < SDC_MIN_SHUTDOWN;
}

bool GRIError(Inverter_Status_Msg_Three *msgGriThree)
{
    if (msgGriThree->fault_map != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "GRIErr%hhX", msgGriThree->fault_map);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);  // Not sending '\0'

        char steeringMsg[16];
        snprintf(steeringMsg, 16, "GRI Error -- %hhX", msgGriThree->fault_map);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 15);   // Not sending '\0'

        LOGOMATIC("GRI Error -- %hhX", msgGriThree->fault_map);

        return true;
    }

    return false;
}

bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo)
{
    uint8_t errorBits = getBits(acuMsgTwo->Error_Warning_Bits, 5, 3);

    if (errorBits != 0x0)
    {
        char dashMsg[9];
        snprintf(dashMsg, 9, "ACUWar%hhx", errorBits);
        writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_DASH_PANEL, (uint8_t*)dashMsg, 8);    // Not sending '\0'

        char steeringMsg[18];
        snprintf(steeringMsg, 18, "ACU Warning -- %hhX", errorBits);
        writeMessage(DataBusCAN, MSG_DEBUG_FD, GR_STEERING_WHEEL, (uint8_t*)steeringMsg, 17);   // Not sending '\0'

        LOGOMATIC("ACU Warning -- %hhX", errorBits);

        return true;
    }

    return false;
}
