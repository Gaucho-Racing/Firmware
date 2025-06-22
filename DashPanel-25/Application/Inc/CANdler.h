#include <stdint.h>

#ifndef CANDLER_H
#define CANDLER_H

extern volatile uint8_t numberOfBadMessages;

void handleDtiCANMessage(uint16_t msgID, uint8_t* data, uint32_t length);
void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length);

typedef struct {
    uint8_t flags;
} DashWarningFlagsMsg;

typedef struct {
    uint8_t ECUState;
    uint8_t StatusBits[3];
    uint8_t PowerLevelTorqueMap;
    uint8_t MaxCellTemp;
    uint8_t AccumulatorStateOfCharge;
    uint8_t GLVStateOfCharge;
} ECUStatusMsgOne;

typedef struct {
    uint16_t TractiveSystemVoltage;
    uint16_t VehicleSpeed;
    uint16_t FRWheelRPM;
    uint16_t FLWheelRPM;
} ECUStatusMsgTwo;

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
    uint16_t AC_Current;
    uint16_t DC_Current;
} DTIDataTwoMsg;

typedef struct {
    uint16_t AC_Current;
    uint16_t DC_Current;
    uint16_t Motor_RPM;
} InverterStatusOne;

typedef struct {
    uint8_t uTemp;
    uint8_t vTemp;
    uint8_t wTemp;
} InverterStatusTwo;

typedef struct {
    uint8_t Motor_Temp;
    uint8_t Inverter_Fault_Map;
} InverterStatusThree;

typedef struct {
    uint8_t Wheel_Identifier;
    uint8_t Brake_Temp;
} SpecificBrakeIR;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} ButtonLedBrightness;

typedef struct {
    uint8_t ledBits;
    ButtonLedBrightness buttonLedOne;
    ButtonLedBrightness buttonLedTwo;
} DashConfigMsg;

#endif
