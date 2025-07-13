#include <stdint.h>

#ifndef CANDLER_H
    #define CANDLER_H

    extern volatile uint8_t globalSteeringStatusRegenButtonMap;

    /**
    Takes in all CAN messages and handles them as they need to be handled.
    The identifiers can represent which bus the packet came in on, if that is needed.

    Follows the `CAN ID Assignments` spreadsheet in the Team Drive.

    @param msgID Message GR ID
    @param srcID Source GR ID
    @param data Binary data array
    @param length Length of the binary data array
    */
    void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length);
    void handleDtiCANMessage(uint16_t msgID, uint8_t* data, uint32_t length);

    typedef enum {
        PrimaryBusCAN = (uint8_t)0,
        DataBusCAN = (uint8_t)1,
    } BusCAN;

    typedef struct {
        uint16_t Accumulator_Voltage;
        uint16_t TS_Voltage;
        uint16_t Accumulator_Current;
        uint8_t Accumulator_SOC;
        uint8_t GLV_SOC;
    } ACU_Status_MsgOne;

    typedef struct {
        uint8_t V_20v_Voltage;
        uint8_t V_12v_Voltage;
        uint8_t SDC_Voltage;
        uint8_t Min_Cell_Voltage;
        uint8_t Max_Cell_Temp;
        uint8_t Error_Warning_Bits;
        uint8_t Precharge_Error_IR_State_Software_Latch_Bits;  // All in one!
    } ACU_Status_MsgTwo;

    typedef struct {
        uint16_t AC_current;
        uint16_t DC_current;
        uint16_t Motor_Rpm;
    } Inverter_Status_Msg_One;

    typedef struct {
        uint8_t Motor_temperature;
        uint8_t fault_map;
    } Inverter_Status_Msg_Three;

    typedef struct {
        uint8_t Current_Torque_Map_Encoder;
        uint8_t regenButtonMap;
    } Steering_Status_Msg;

    typedef struct {
        uint8_t Led_Map;
        int8_t TSButtonData;
        int8_t RTDButtonData;
    } Dash_Status_Msg;

    typedef struct {
        uint32_t erpm;
        uint16_t dutyCycle;
        uint16_t inputVoltage;
    } DTI_Data_Msg_One;

    typedef struct {
        uint16_t acVoltage;
        uint16_t dcVoltage;
        uint32_t RESERVED_TWO;
    } DTI_Data_Msg_Two;

    typedef struct {
        uint16_t controllerTemp;
        uint16_t motorTemp;
        uint8_t faultCodes;
        uint8_t RESERVED_THREE[3];
    } DTI_Data_Msg_Three;

    typedef struct {
        uint32_t focId;
        uint32_t focIq;
    } DTI_Data_Msg_Four;

    typedef struct {
        uint8_t throttle;
        uint8_t brake;
        uint8_t digitalInput;
        uint8_t driveEnable;
        uint8_t variousLimitFlags[2];
        uint8_t RESERVED_FIVE;
        uint8_t canVersion;
    } DTI_Data_Msg_Five;

    typedef struct {
        DTI_Data_Msg_One msgOne;
        DTI_Data_Msg_Two msgTwo;
        DTI_Data_Msg_Three msgThree;
        DTI_Data_Msg_Four msgFour;
        DTI_Data_Msg_Five msgFive;
    } DTI_Data;

    typedef struct {
        uint8_t temp;
    } SAM_Brake_IR_Msg;

    typedef struct {
        uint8_t id;
        uint8_t temp;
    } Specific_Brake_IR_Msg;

#endif // CANDLER_H
