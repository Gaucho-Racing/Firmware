#include <stdint.h>
#include <stdbool.h>

#include "adc.h"

#ifndef STATEMACHINE_H
    #define STATEMACHINE_H

    #define HOW_LONG_TSDISCHARGE_UNTIL_ERROR_MS 5000
    #define SDC_MIN_SHUTDOWN 50
    #define TS_VOLTAGE_OFF_LIMIT 60
    #define BAD_MESSAGE_LIMIT 3
    #define BAD_TIME_Negative1 -1

    typedef enum {
        GLV_ON = (uint8_t)1,
        PRECHARGE_ENGAGED = (uint8_t)2,
        PRECHARGING = (uint8_t)3,
        PRECHARGE_COMPLETE = (uint8_t)4,
        DRIVE_STANDBY = (uint8_t)5,
        DRIVE_ACTIVE_IDLE = (uint8_t)6,
        DRIVE_ACTIVE_POWER = (uint8_t)7,
        DRIVE_ACTIVE_REGEN = (uint8_t)8,
        TS_DISCHARGE_OFF = (uint8_t)9,
        REFLASH_TUNE = (uint8_t)10,
        ERRORSTATE = (uint8_t)11,
    } State;

    typedef union {
        struct {
            uint8_t ECUState;
            uint8_t StatusBits[3];
            uint8_t PowerLevelTorqueMap;
            uint8_t MaxCellTemp;
            uint8_t AccumulatorStateOfCharge;
            uint8_t GLVStateOfCharge;

            uint16_t TractiveSystemVoltage;
            uint16_t VehicleSpeed;
            int16_t FRWheelRPM;
            int16_t FLWheelRPM;

            int16_t RRWheelRPM;
            int16_t RLWheelRPM;

            uint16_t APPS1_SIGNAL;
            uint16_t APPS2_SIGNAL;
            uint16_t AUX_SIGNAL;
            uint16_t BRAKE_F_SIGNAL;
            uint16_t BRAKE_R_SIGNAL;
        };

        struct {
            uint8_t ECUStatusMsgOne[8];
            uint8_t ECUStatusMsgTwo[8];
            uint8_t ECUStatusMsgThree[4];
            uint8_t ECUPedalDataMsg[10];
        };
    } StatusLump;

    extern volatile StatusLump globalStatus;
    extern volatile uint8_t numberOfBadMessages;

    bool determineSignalForDashLEDs(AnalogSignal sig);

    /**
    Scales `globalStatus` and returns a copy that is scaled.
    
    @returns A StatusLump struct that follows the CAN ID Assignment spreadsheet

    Does not change globalStatus in any way/shape/form.
    */
    StatusLump scaledECUStatusMsgForTx(void);

    /**
    General Low Voltage - On

    Once the GLVMS is set to ON this state is reached de facto.
    This is the first state reached on the board recieving power.
    The reciprocal off state is when the board has no power.
    Should a terrible error occur we may be thrown back here.

    However, we may configure boot to jump to `REFLASH_TUNE` instead.
    That is pending development. The other thought is to configure via CAN.

    Pretty much holds state until a CAN message comes in.
    */
    void glv_on(void);

    /**
    Precharge Engaged

    Once in `GLV_ON` and the TSMS is set to ON.

    Pretty much holds state.
    */
    void precharge_engaged(void);

    /**
    Precharging

    Once in `PRECHARGE_ENGAGED` and the ACU gives a precharge confirmation.

    Pretty much holds state.
    */
    void precharging(void);

    /**
    Precharge Complete

    Once in `PRECHARGING` and the ACU gives a precharge success confirmation.

    Pretty much holds state.
    */
    void precharge_complete(void);

    /**
    TD Discharge Off

    Complicated. Refer to the actual code for the best understanding.

    Once TSV is higher than TS_VOLTAGE_OFF_LIMIT volts and we leave drive state for any reason.
    Discharges the TS so that HV systems can be safe iff there are no errors.
    Can be errorful or standardly running when this state is reached.

    Tries to reduce the HV TS if it is possible.
    */
    void ts_discharge_off(void);

    /**
    Reflash Tune

    Might read from the SD card on startup and configure some settings.
    Could backup parameters, change parameters, validate parameters, etc.

    Pending implementation. Passes to `GLV_ON` on success.
    */
    void reflash_tune(void);

    /**
    Error

    Complicated. Refer to the actual code for the best understanding.

    Attempts to send to discharge if TSV > TS_VOLTAGE_OFF_LIMIT volts.

    Pretty much holds state until errors resolved.
    */
    void error(void);

    /**
    State Machine Tick

    Call as often as possible, handles all logic and internal systems.
    Effectively half of the ECU, works with the interrupt-focused `CANDler.c`.

    Ticks the state machine and calls the appropriate function.
    */
    void stateMachineTick(void);

#endif // STATEMACHINE_H
