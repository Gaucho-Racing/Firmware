#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "adc.h"
#include "fdcan.h"
#include "StateData.h"
#include "msgIds.h"
#include "StateUtils.h"
#include "StateTicks.h"


// TODO: REMOVE THESE GLOBALS AND PUT THEM IN STATEDATA
volatile uint8_t errorFlagBitsCan = 0;
volatile uint8_t globalSteeringStatusRegenButtonMap = 0;

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

            //LOGOMATIC("%.*s", (int)length, (const char*)data);
            //writeMessage(DataBusCAN, MSG_DEBUG_2_0, GR_STEERING_WHEEL, data, length);

            break;

        case MSG_DEBUG_FD:
            if (length > 64) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            //LOGOMATIC("%.*s", (int)length, (const char*)data);
            //writeMessage(DataBusCAN, MSG_DEBUG_2_0, GR_STEERING_WHEEL, data, length);

            break;

        case MSG_PING:
            if (length != 4) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            //LOGOMATIC("Got pinged by %X", srcID);
            //respondToPing(srcID, *(uint32_t*)data);

            break;

        case MSG_ACU_STATUS_1:
            if (length != 8) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            /*
            ACU_Status_MsgOne* acuMsgOne = (ACU_Status_MsgOne*)data;
            stateData->AccumulatorStateOfCharge = (uint8_t)(acuMsgOne->Accumulator_SOC * 20.0 / 51.0);
            stateData->GLVStateOfCharge = (uint8_t)(acuMsgOne->GLV_SOC * 20.0 / 51.0);
            stateData->TractiveSystemVoltage = (uint8_t)(acuMsgOne->TS_Voltage * 0.01);
            */

            break;

        case MSG_ACU_STATUS_2:
            if (length != 7) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            /*

            ACU_Status_MsgTwo* acuMsgTwo = (ACU_Status_MsgTwo*)data;
            stateData->MaxCellTemp = (uint8_t)(acuMsgTwo->Max_Cell_Temp * 0.25);

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

            */
            //stateData->PrechargeSuccess = getBits(acuMsgTwo->Precharge_Error_IR_State_Software_Latch_Bits, 0, 4) == 0x07 // 111 -> software latch, AIR- and AIR+ all closed
            

            break;

        case MSG_INVERTER_STATUS_1:
            if (length != 6) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            //we don't have four motors anymore
            /*
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
                    */
            break;

        case MSG_INVERTER_STATUS_3:
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            //Safety stuff in inverter message. Needs reword

            /*
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
                */

            break;

        case MSG_DASH_STATUS:
            if (length != 3) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            /*
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
          //  }

            break;

        case MSG_STEERING_STATUS:
            if (length != 2) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }
            /*
            Steering_Status_Msg* msgSteer = (Steering_Status_Msg*)data;
            globalStatus.PowerLevelTorqueMap = msgSteer->Current_Torque_Map_Encoder;

            /* Do not write to these values elsewhere! 
            globalSteeringStatusRegenButtonMap = msgSteer->regenButtonMap;
            */
            break;

        case MSG_SAM_BRAKE_IR:
            if (length != 1) {
                numberOfBadMessages++;
                return;
            } else {
                numberOfBadMessages += (numberOfBadMessages > 0) ? -1 : 0;
            }

            /*
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
            */

            break;

        #ifdef ADVANCED_LOGGING
            default:
                LOGOMATIC("Got msg %X from %X of length %d", msgID, srcID, (int)length);
                return;
        #endif
    }
}