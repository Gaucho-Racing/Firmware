#include <stdbool.h>

#include "stm32g4xx_hal.h"
#include "stateMachine.h"
#include "driving.h"
#include "inverter.h"
#include "main.h"
#include "utils.h"
#include "adc.h"
#include "fdcan.h"
#include "msgIDs.h"
#include "grIDs.h"

volatile StatusLump globalStatus = {
    .ECUState = GLV_ON,
    .StatusBits = {0},
    .PowerLevelTorqueMap = POWERLEVEL_TORQUEMAP_RESET,
    .MaxCellTemp = 0x0,
    .AccumulatorStateOfCharge = 0x0,
    .GLVStateOfCharge = 0x0,
};

volatile uint8_t numberOfBadMessages = 0;
int32_t dischargeStartMillis = BAD_TIME_Negative1;
volatile uint32_t lastECUStatusMsgTick = 0;
volatile uint32_t lastTSSIFlash = 0;

static const uint16_t howOftenToSpamECUStatusMsgs = 50;
static const uint32_t TSSIFlashCycleLength = 400;

bool determineSignalForDashLEDs(AnalogSignal sig)
{
    return ((float*)(&globalAnalog))[sig] >= (((2 << 16) * 0.4f) / 3.3f);
    //                           ^          ^       ^
    //                           |          |       Reference voltage
    //                           |          Analog input voltage threshold
    //                           2^(Number of bits in ADC)
}


void stateMachineTick(void)
{
    HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, determineBrakeLight());

    if (numberOfBadMessages > BAD_MESSAGE_LIMIT)
    {
        Error_Handler();
    }

    if (globalStatus.ECUState != TS_DISCHARGE_OFF)
    {
        dischargeStartMillis = BAD_TIME_Negative1;
    }

    if (globalStatus.ECUState != ERRORSTATE)
    {
        HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_RESET);
    }
    else if (HAL_GetTick() - lastTSSIFlash < TSSIFlashCycleLength / 2)
    {
        HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_SET);
    }
    else
    {
        if (HAL_GetTick() - lastTSSIFlash > TSSIFlashCycleLength)
        {
            lastTSSIFlash = HAL_GetTick();
        }

        HAL_GPIO_WritePin(TSSI_G_CONTROL_GPIO_Port, TSSI_G_CONTROL_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TSSI_R_CONTROL_GPIO_Port, TSSI_R_CONTROL_Pin, GPIO_PIN_RESET);
    }

    switch(globalStatus.ECUState)
    {
        case GLV_ON:
            glv_on();
            break;
        case PRECHARGE_ENGAGED:
            precharge_engaged();
            break;
        case PRECHARGING:
            precharging();
            break;
        case PRECHARGE_COMPLETE:
            precharge_complete();
            break;
        case DRIVE_STANDBY:
            drive_standby();
            break;
        case DRIVE_ACTIVE_IDLE:
            drive_active_idle();
            break;
        case DRIVE_ACTIVE_POWER:
            drive_active_power();
            break;
        case DRIVE_ACTIVE_REGEN:
            drive_active_regen();
            break;
        case TS_DISCHARGE_OFF:
            ts_discharge_off();
            break;
        case REFLASH_TUNE:
            reflash_tune();
            break;
        case ERRORSTATE:
        default:
            error();
        break;
    }

    StatusLump correctlyScaledValues = scaledECUStatusMsgForTx();

    if (HAL_GetTick() - lastECUStatusMsgTick > howOftenToSpamECUStatusMsgs)
    {
        #ifdef ADVANCED_LOGGING
        LOGOMATIC("Sending ECU data packets\n");
        #endif

        uint8_t dashConfigMsg[7] = {0};     // Other bytes not used
        dashConfigMsg[0] = determineSignalForDashLEDs(AMS_SENSE) | (determineSignalForDashLEDs(IMD_SENSE) << 1) | (determineSignalForDashLEDs(BSPD_SENSE) << 2);
        writeMessage(PrimaryBusCAN, MSG_DASH_CONFIG, GR_DASH_PANEL, dashConfigMsg, 7);

        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_1, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgOne, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_2, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgTwo, 8);
        writeMessage(PrimaryBusCAN, MSG_ECU_STATUS_3, GR_ALL, (uint8_t*)correctlyScaledValues.ECUStatusMsgThree, 4);
        writeMessage(PrimaryBusCAN, MSG_DASH_WARNING_FLAGS, GR_DASH_PANEL, (uint8_t*)&BSE_APPS_violation, 1);
        writeMessage(DataBusCAN, MSG_ECU_PEDALS_DATA, GR_TCM, correctlyScaledValues.ECUPedalDataMsg, 10);

        lastECUStatusMsgTick = HAL_GetTick();

        #ifdef ADVANCED_LOGGING
        LOGOMATIC("\n--Global Status Dump--\nECU State %d\n", correctlyScaledValues.ECUState);
        LOGOMATIC("Status Bits 1 %X%X%X\n", correctlyScaledValues.StatusBits[0], correctlyScaledValues.StatusBits[1], correctlyScaledValues.StatusBits[2]);
        LOGOMATIC("Power Level & Torque Map %X\n", correctlyScaledValues.PowerLevelTorqueMap);
        LOGOMATIC("Max Cell Temp %d\n", correctlyScaledValues.MaxCellTemp);
        LOGOMATIC("Accumulator SoC %d\n", correctlyScaledValues.AccumulatorStateOfCharge);
        LOGOMATIC("GLV SoC %d\n", correctlyScaledValues.GLVStateOfCharge);
        LOGOMATIC("TS Voltage %d\n", correctlyScaledValues.TractiveSystemVoltage);
        LOGOMATIC("Vehicle Speed %d\n", correctlyScaledValues.VehicleSpeed);
        LOGOMATIC("FR Wheel RPM %d\n", correctlyScaledValues.FRWheelRPM);
        LOGOMATIC("FL Wheel RPM %d\n", correctlyScaledValues.FLWheelRPM);
        LOGOMATIC("RR Wheel RPM %d\n", correctlyScaledValues.RRWheelRPM);
        LOGOMATIC("RL Wheel RPM %d\n", correctlyScaledValues.RLWheelRPM);
        LOGOMATIC("--Global Status Dump--\n");
        #endif
    }
}

StatusLump scaledECUStatusMsgForTx(void)
{
    StatusLump scaledStatus = globalStatus;

    scaledStatus.MaxCellTemp *= 4;
    scaledStatus.AccumulatorStateOfCharge = (uint8_t)(scaledStatus.AccumulatorStateOfCharge * 51.0f / 20.0f);
    scaledStatus.GLVStateOfCharge *= (uint8_t)(scaledStatus.GLVStateOfCharge * 51.0f / 20.0f);
    scaledStatus.TractiveSystemVoltage *= 100;
    scaledStatus.VehicleSpeed *= 100;
    scaledStatus.FLWheelRPM = scaledStatus.FLWheelRPM * 10 + 32768;
    scaledStatus.FRWheelRPM = scaledStatus.FRWheelRPM * 10 + 32768;
    scaledStatus.RLWheelRPM = scaledStatus.RLWheelRPM * 10 + 32768;
    scaledStatus.RRWheelRPM = scaledStatus.RRWheelRPM * 10 + 32768;
    scaledStatus.APPS1_SIGNAL = 65535 * (scaledStatus.APPS1_SIGNAL - THROTTLE_MIN_1) / (THROTTLE_MAX_1 - THROTTLE_MIN_1); // Don't reverse multiplication here
    scaledStatus.APPS2_SIGNAL = 65535 * (scaledStatus.APPS2_SIGNAL - THROTTLE_MIN_2) / (THROTTLE_MAX_2 - THROTTLE_MIN_2);
    scaledStatus.AUX_SIGNAL = 65535 * (scaledStatus.APPS2_SIGNAL - AUX_MIN) / (AUX_MAX - AUX_MIN);
    scaledStatus.BRAKE_F_SIGNAL = 65535 * (scaledStatus.APPS2_SIGNAL - BRAKE_F_MIN) / (BRAKE_F_MAX - BRAKE_F_MIN);
    scaledStatus.BRAKE_R_SIGNAL = 65535 * (scaledStatus.APPS2_SIGNAL - BRAKE_R_MIN) / (BRAKE_R_MAX - BRAKE_R_MIN);

    return scaledStatus;
}

void glv_on(void)
{
    // For safety
    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        globalStatus.ECUState = TS_DISCHARGE_OFF;
        return;
    }

    // Close software latch, should be error free at this point, also reset power level
    setSoftwareLatch(true);
    globalStatus.PowerLevelTorqueMap = POWERLEVEL_TORQUEMAP_RESET;
    // TS on handled in CANdler -> MSG_DASH_STATUS
}

void precharge_engaged(void)
{
    // For safety
    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        LOGOMATIC("Tripped TS Voltage in precharge engaged state, discharging\n");
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    // ACU confirmation is IR-, handled in CANdler
    //TS ACTIVE botton disabled --> GLV_ON is handled in CANdler
}

void precharging(void)
{
    //TS ACTIVE button disabled || ACU precharge cancellation --> TS_DISCHARGE_OFF is handled in CANdler line 66, 176
}

void precharge_complete(void)
{
    // If front, rear, and rtd, then go to DRIVE_STANDBY handled in CANdler
    // TS ACTIVE, ACU shutdown, errors handled in CANdler
}


void ts_discharge_off(void)
{
    setSoftwareLatch(false);
    controlInverters(false);

    if (dischargeStartMillis == BAD_TIME_Negative1)
    {
        dischargeStartMillis = millis();
    }
    
    if ((millis() - dischargeStartMillis) > HOW_LONG_TSDISCHARGE_UNTIL_ERROR_MS)
    {
        LOGOMATIC("Discharging too long, jumping to ERRORSTATE\n");
        globalStatus.ECUState = ERRORSTATE;  // ERRORSTATE will send it back if voltage >= TS_VOLTAGE_OFF_LIMIT
    }
}

void reflash_tune(void)
{
    // Legacy, controlled via encoders and reflashing and such
    globalStatus.ECUState = GLV_ON;
}

void error(void)
{
    setSoftwareLatch(false);
    controlInverters(false);

    if (globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
    {
        LOGOMATIC("Error at too high a TS Voltage, jumping to TS_DISCHARGE_OFF\n");
        globalStatus.ECUState = TS_DISCHARGE_OFF;
    }

    /* Only error resolved when MSG_ACU AND GRI says we are good -> Handled in CANdler */
}
