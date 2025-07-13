#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "CANdler.h"
#include "stateMachine.h"
#include "msgIDs.h"
#include "pinging.h"
#include "driving.h"
#include "inverter.h"
#include "customIDs.h"
#include "adc.h"
#include "fdcan.h"
#include "grIDs.h"
#include "utils.h"

volatile uint8_t errorFlagBitsCan = 0;
volatile uint8_t globalSteeringStatusRegenButtonMap = 0;
volatile bool prevRTD = 1;
volatile bool prevTS_ON = 1;

void handleDtiCANMessage(uint16_t msgID, uint8_t* data, uint32_t length)
{
    #ifdef ADVANCED_LOGGING
    LOGOMATIC("Recieved a CAN message from the DTI!\tMessage ID: %d\tLength: %ld\n", msgID, length);
    #endif

    if (length != 8) {
        numberOfBadMessages++;
        return;
    } else {
        numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
    }

    switch(msgID)
    {
        case MSG_DTI_DATA_1:
            globalInverterData.msgOne = *(DTI_Data_Msg_One*)data;
            globalStatus.VehicleSpeed = (uint16_t)vehicleSpeedMPH();
            break;

        case MSG_DTI_DATA_2:
            globalInverterData.msgTwo = *(DTI_Data_Msg_Two*)data;
            break;

        case MSG_DTI_DATA_3:
            globalInverterData.msgThree = *(DTI_Data_Msg_Three*)data;
            break;

        case MSG_DTI_DATA_4:
            globalInverterData.msgFour = *(DTI_Data_Msg_Four*)data;
            break;

        case MSG_DTI_DATA_5:
            globalInverterData.msgFive = *(DTI_Data_Msg_Five*)data;
            break;

        default:
            return;
    }
}

void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length)
{
    #ifdef ADVANCED_LOGGING
    LOGOMATIC("Recieved a CAN message!\tMessage ID: %d\tLength: %ld\n", msgID, length);
    #endif

    switch(msgID)
    {
        case MSG_DEBUG_2_0:
            if (length > 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            LOGOMATIC("%.*s", (int)length, (const char*)data);
            writeMessage(DataBusCAN, MSG_DEBUG_2_0, GR_STEERING_WHEEL, data, length);

            break;

        case MSG_DEBUG_FD:
            if (length > 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            LOGOMATIC("%.*s", (int)length, (const char*)data);
            writeMessage(DataBusCAN, MSG_DEBUG_2_0, GR_STEERING_WHEEL, data, length);

            break;

        case MSG_PING:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            LOGOMATIC("Got pinged by %X", srcID);
            respondToPing(srcID, *(uint32_t*)data);

            break;

        case MSG_ACU_STATUS_1:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgOne* acuMsgOne = (ACU_Status_MsgOne*)data;
            globalStatus.AccumulatorStateOfCharge = (uint8_t)(acuMsgOne->Accumulator_SOC * 20.0 / 51.0);
            globalStatus.GLVStateOfCharge = (uint8_t)(acuMsgOne->GLV_SOC * 20.0 / 51.0);
            globalStatus.TractiveSystemVoltage = (uint8_t)(acuMsgOne->TS_Voltage * 0.01);

            break;

        case MSG_ACU_STATUS_2:
            if (length != 7) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            ACU_Status_MsgTwo* acuMsgTwo = (ACU_Status_MsgTwo*)data;
            globalStatus.MaxCellTemp = (uint8_t)(acuMsgTwo->Max_Cell_Temp * 0.25);

            // errorFlagBitsCan logic
            if (ACUError(acuMsgTwo) && (errorFlagBitsCan == 0 || errorFlagBitsCan == 2))
            {
                LOGOMATIC("ACU Msg 2 tripped ACU Error: 'ACUError(acuMsgTwo) && (errorFlagBitsCan == 0 || errorFlagBitsCan == 2)'\n");
                errorFlagBitsCan += 1;
            }
            else if (!ACUError(acuMsgTwo) && (errorFlagBitsCan == 1 || errorFlagBitsCan == 3))
            {
                LOGOMATIC("ACU Msg 2 tripped errorFlagBitsCan: '!ACUError(acuMsgTwo) && (errorFlagBitsCan == 1 || errorFlagBitsCan == 3)'\n");
                errorFlagBitsCan -= 1;
            }

            if (ACUWarning(acuMsgTwo))
            {
                LOGOMATIC("ACU Msg 2 --- Under Voltage!\n");
                globalStatus.PowerLevelTorqueMap = (globalStatus.PowerLevelTorqueMap >> 5 << 4) | (globalStatus.PowerLevelTorqueMap << 4 >> 4);
                writeMessage(PrimaryBusCAN, MSG_DEBUG_2_0, GR_ALL, (uint8_t*)"UnderVol", 8);
            }

            //Error handling and leaving error state
            if ((errorFlagBitsCan || getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0) == 0x1) && globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
            {
                LOGOMATIC("Entering TS Discharge Off!\n");
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if (errorFlagBitsCan || getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0) == 0x1)
            {
                LOGOMATIC("Safe to enter ERRORSTATE\n");
                globalStatus.ECUState = ERRORSTATE;
            }
            else if (globalStatus.ECUState == ERRORSTATE || (globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < TS_VOLTAGE_OFF_LIMIT))
            {
                LOGOMATIC("Cleared, re-entering GLV ON\n");
                prevTS_ON = 1;
                globalStatus.ECUState = GLV_ON;
            }

            switch(globalStatus.ECUState)
            {
                case GLV_ON:
                    break;

                case PRECHARGE_ENGAGED:
                    if (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x1)
                    {
                        LOGOMATIC("Entering PRECHARGING\n");
                        globalStatus.ECUState = PRECHARGING;
                    }

                    break;

                case PRECHARGING:
                    if (getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 3) == 0x06/*getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 4) == 0x0E*/)
                    {
                        LOGOMATIC("Entering PRECHARGE COMPLETE\n");
                        globalStatus.ECUState = PRECHARGE_COMPLETE;
                    }

                    // DO NOT DELETE THIS FALLTHROUGH
                    __attribute__((fallthrough));
                    // Theres not supposed to be a break here !!!! Stop adding it back

                default:
                    if (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x0 || ((getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 2) == 0x00) && globalStatus.ECUState != PRECHARGING))
                    {
                        LOGOMATIC("ACU Msg 2 tripped some precharge error IR state software latch bit stuff\n");
                        //guys why is ir- open  and ir+ open && !precharge
                        // Switch case: it will be past precharge engaged, relays opening is bad
                         globalStatus.ECUState = TS_DISCHARGE_OFF;
                    }
            }

            //(If ACU software latch ever opens or )IR- ever opens while IR+ is closed, something has gone wrong
            if (getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1, 2) == 0x02 /*|| (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 3) == 0x00 && globalStatus.ECUState > PRECHARGE_ENGAGED && globalStatus.ECUState <= DRIVE_ACTIVE_REGEN)*/)
            {
                LOGOMATIC("ACU software latch opens or IR- opened while IR+ is closed\tEntering TS Discharge Off\n");
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
/*
            // IR- -> 1 = ACU Precharge Confirmation
            if ((globalStatus.ECUState == PRECHARGE_ENGAGED) && (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x1))
            {
                globalStatus.ECUState = PRECHARGING;
            }

            // If IR- ever becomes 0 while not in GLV_ON or PRECHARGE_ENGAGED, that is a precharge cancellation and it must start discharging.
            if (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1) == 0x0 && (globalStatus.ECUState != GLV_ON) && (globalStatus.ECUState != PRECHARGE_ENGAGED))
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            // If it is precharging with IR- closed and then IR+ goes closed as well without precharge error, precharge is complete (success confirmation)
            // IR+ -> 1 is precharge success confirmation
            if ((getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 4) == 0x07) && (globalStatus.ECUState == PRECHARGING))
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }

            //If IR+ ever opens on or after the precharging complete state, start discharging
            if (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 2) == 0x00 && globalStatus.ECUState != GLV_ON && globalStatus.ECUState != PRECHARGE_ENGAGED && globalStatus.ECUState != PRECHARGING)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }

            //If ACU software latch ever opens or IR- ever opens while IR+ is closed, something has gone wrong
            if (getBit(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 3) == 0x00 || getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 1, 2) == 0x01)
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
*/
            // USE ACUWarning(acuMsgTwo) HERE FOR DASH WARNINGS

            break;

        case MSG_INVERTER_STATUS_1:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_One* msgGriOne = (Inverter_Status_Msg_One*)data;

            switch (srcID)
            {
                case GR_GR_INVERTER_1:
                    globalStatus.RLWheelRPM = (int16_t)(msgGriOne->Motor_Rpm * 0.1 - 3279);
                    break;
                case GR_GR_INVERTER_2:
                    globalStatus.RRWheelRPM = (int16_t)(msgGriOne->Motor_Rpm * 0.1 - 3279);
                    break;
                case GR_GR_INVERTER_3:
                    globalStatus.FLWheelRPM = (int16_t)(msgGriOne->Motor_Rpm * 0.1 - 3279);
                    break;
                case GR_GR_INVERTER_4:
                    globalStatus.FRWheelRPM = (int16_t)(msgGriOne->Motor_Rpm * 0.1 - 3279);
                    break;
            }

            break;

        case MSG_INVERTER_STATUS_3:
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Inverter_Status_Msg_Three* msgGriThree = (Inverter_Status_Msg_Three*)data;

            if (GRIError(msgGriThree) && (errorFlagBitsCan == 0 || errorFlagBitsCan == 1))
            {
                LOGOMATIC("Tripped GRI Error\n");
                errorFlagBitsCan += 2;
            }
            else if (!GRIError(msgGriThree) && (errorFlagBitsCan == 2 || errorFlagBitsCan == 3))
            {
                LOGOMATIC("Tripped error flag bits can\n");
                errorFlagBitsCan -= 2;
            }

            if (errorFlagBitsCan && globalStatus.TractiveSystemVoltage >= TS_VOLTAGE_OFF_LIMIT)
            {
                LOGOMATIC("Inverter Status 3 Msg tripped us to TS Discharge Off\n");
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if (errorFlagBitsCan)
            {
                LOGOMATIC("Error flag bits on CAN so going to ERRORSTATE\n");
                globalStatus.ECUState = ERRORSTATE;
            }
            else if (globalStatus.ECUState == ERRORSTATE || (globalStatus.ECUState == TS_DISCHARGE_OFF && globalStatus.TractiveSystemVoltage < TS_VOLTAGE_OFF_LIMIT))
            {
                LOGOMATIC("Errors cleared, jumping to GLV ON\n");
                prevTS_ON = 1;
                globalStatus.ECUState = GLV_ON;
            }

            break;

        case MSG_DASH_STATUS:
            if (length != 3) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Dash_Status_Msg *dashStatusMsg = (Dash_Status_Msg*)data;
            bool ts_on = dashStatusMsg->TSButtonData;
            bool rtd = dashStatusMsg->RTDButtonData;

            HAL_GPIO_WritePin(RTD_CONTROL_GPIO_Port, RTD_CONTROL_Pin, rtd);

            switch(globalStatus.ECUState)
            {
                case GLV_ON:
                    if (determineSignalForDashLEDs(BSPD_SENSE))
                    {
                        LOGOMATIC("Driver should not press both pedals at the same time\n");
                        break;  // Tell driver to not press both pedals at the same time to charge the car
                    }

                    if (!ts_on)
                    {
                        prevTS_ON = 0;
                    }
                    else if (!prevTS_ON)
                    {
                        LOGOMATIC("TS Active engaged rising edge\n");
                        globalStatus.ECUState = PRECHARGE_ENGAGED;
                        prevTS_ON = 1;
                    }

                    break;

                case PRECHARGE_ENGAGED:
                    if (!ts_on)
                    {
                        LOGOMATIC("Jumping to GLV ON\n");
                        globalStatus.ECUState = GLV_ON;
                        writeMessage(PrimaryBusCAN, MSG_ACU_PRECHARGE, GR_ACU, (uint8_t*)&ts_on, 1);
                    }

                    break;

                case PRECHARGING:
                    if (rtd)
                    {
                        LOGOMATIC("Tell driver to not press RTD until precharge complete\n");
                    }

                    if (!ts_on)
                    {
                        LOGOMATIC("Gotta keep TS Active on to continue charging\n");
                        globalStatus.ECUState = TS_DISCHARGE_OFF;
                        writeMessage(PrimaryBusCAN, MSG_ACU_PRECHARGE, GR_ACU, (uint8_t*)&ts_on, 1);
                    }

                    break;

                case PRECHARGE_COMPLETE:
                    if (rtd && !prevRTD && pressingBrake())
                    {
                        LOGOMATIC("Promoted to Drive Standby\n");
                        globalStatus.ECUState = DRIVE_STANDBY;
                        prevRTD = 1;
                    }

                    else if(!rtd && prevRTD && pressingBrake())
                    {
                        LOGOMATIC("Ready to press RTD\n");
                        prevRTD = 0;
                    }

                    else if(!pressingBrake()){
                        LOGOMATIC("Brake not depressed\n");
                        prevRTD = 1; // Rising edge must happen after and while driver presses brake.
                    }

                    if (!ts_on)
                    {
                        LOGOMATIC("Keep TS Active on to keep it on\n");
                        globalStatus.ECUState = TS_DISCHARGE_OFF;
                        writeMessage(PrimaryBusCAN, MSG_ACU_PRECHARGE, GR_ACU, (uint8_t*)&ts_on, 1);
                    }

                    break;

                default:
                    if (!ts_on)
                    {
                        LOGOMATIC("TS must be kept alive\n");
                        globalStatus.ECUState = TS_DISCHARGE_OFF;
                        writeMessage(PrimaryBusCAN, MSG_ACU_PRECHARGE, GR_ACU, (uint8_t*)&ts_on, 1);
                    }

                    if (!rtd)
                    {
                        LOGOMATIC("RTD must be kept alive\n");
                        globalStatus.ECUState = PRECHARGE_COMPLETE;
                    }
/*
            if (globalStatus.ECUState == GLV_ON)
            {
                if (ts_on)
                {
                    globalStatus.ECUState = PRECHARGE_ENGAGED;
                }
            }            
            else if (!ts_on && globalStatus.ECUState == PRECHARGE_ENGAGED)
            {
                globalStatus.ECUState = GLV_ON;
            }
            else if (!ts_on && globalStatus.ECUState != ERRORSTATE) // If it is not in GLV_ON, PRECHARGE_ENGAGED or ERRORSTATE, if ts_off is ever true it must go to discharge
            {
                globalStatus.ECUState = TS_DISCHARGE_OFF;
            }
            else if (globalStatus.ECUState == PRECHARGE_COMPLETE && rtd && globalAnalog.BRAKE_F_SIGNAL > 100 && globalAnalog.BRAKE_R_SIGNAL > 100)
            {
                globalStatus.ECUState = DRIVE_STANDBY;
            }
            else if (globalStatus.ECUState == DRIVE_STANDBY && !rtd)
            {
                globalStatus.ECUState = PRECHARGE_COMPLETE;
            }
*/
            }

            break;

        case MSG_STEERING_STATUS:
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Steering_Status_Msg* msgSteer = (Steering_Status_Msg*)data;
            globalStatus.PowerLevelTorqueMap = msgSteer->Current_Torque_Map_Encoder;

            /* Do not write to these values elsewhere! */
            globalSteeringStatusRegenButtonMap = msgSteer->regenButtonMap;
            break;

        case MSG_SAM_BRAKE_IR:
            if (length != 1) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            Specific_Brake_IR_Msg brakeIrMsg;
            brakeIrMsg.temp = ((Specific_Brake_IR_Msg*)data)->temp;
            
            switch (srcID) {
                case GR_SAM1:
                    brakeIrMsg.id = 0;
                    break;
                case GR_SAM2:
                    brakeIrMsg.id = 1;
                    break;
                case GR_SAM3:
                    brakeIrMsg.id = 2;
                    break;
                case GR_SAM4:
                    brakeIrMsg.id = 3;
                    break;
            }

            writeMessage(PrimaryBusCAN, MSG_SPECIFIC_BRAKE_IR, GR_STEERING_WHEEL, (uint8_t*)&brakeIrMsg, 2);

            break;

        #ifdef ADVANCED_LOGGING
            default:
                LOGOMATIC("Got msg %X from %X of length %d", msgID, srcID, (int)length);
                return;
        #endif
    }
}
