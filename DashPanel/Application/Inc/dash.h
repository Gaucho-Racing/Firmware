#include <stdint.h>

#include "CANdler.h"
#include "gui.h"

#ifndef DASH_H
    #define DASH_H

    #define BAD_TIME_Negative1 -1
    #define DASH_STATUS_MSG_DELAY 100
    #define DISPLAY_DEBUG_MESSAGE_TIME_MS 6000

    extern volatile bool recievedNewInformationPleaseRefresh;

    /* Must match ECU-25 EXACTLY */
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
    } ECUState;

    extern const char* ECUStateNames[];
    
    typedef struct {
        uint8_t currentEncoder;
        uint8_t torqueMapEncoder;
        uint8_t regenEncoder;
    } SteeringStatusMsg;

    typedef struct {
        uint8_t ledBits;
        int8_t tsButtonData;
        int8_t rtdButtonData;
    } DashStatusMsg;

    typedef struct {
        DashStatusMsg dashStatusMsg;
        SteeringStatusMsg steeringStatusMsg;

        ButtonLedBrightness buttonLedOne;
        ButtonLedBrightness buttonLedTwo;
        uint8_t bseAppsViolation;
        uint8_t maxCellTemp; // Max cell temp from entire car
        uint8_t glvStateOfCharge; // SOC for LV
        uint8_t motorTemperatures[3]; // Temperatures for all 4(?) motors X 
        uint8_t inverterTemperatures[3]; // Temepratures for all 4(?) inverters
        uint8_t brakeTemps[4]; // Temperatures for all 4 brakes
        
        uint16_t vehicleSpeed; // From ECU Status 2
        ECUState ecuState; // Current state of the car
        uint16_t tsVoltage;
        uint8_t accumulatorStateOfCharge; // SOC aka battery life
        // power ? (i'm just trying to match up order with gui.h VOLATILE_OBJECTS)
        uint16_t inverterCurrents[3]; // Currents for all 4(?) motors; note: only the first two will be used if the DTI is used X 
        // torque mapping ?
        // regen ?

        char debugMessage[9];   // Will be empty if no message sent, 9th byte always '\0'
    } DashInfo;

    extern volatile DashInfo globalStatus;

    extern uint16_t speedData;
    extern ECUState stateData;
    extern uint16_t voltageData;
    extern uint16_t SoCData;
    extern uint16_t powerData;

    extern char speedBuffer[32];
    extern char stateBuffer[32];
    extern char voltageBuffer[32];  
    extern char SoCBuffer[32];  
    extern char powerBuffer[32];  
    extern char cellBuffer[32];
    extern char motorBuffer[64];
    extern char inverterBuffer[64];
    extern char brakeBuffer[64];

    extern char currentBuffer[32];
    extern char torqueMappingBuffer[32];
    extern char regenBuffer[32];

    void updateWheelDisp(void);
    void updateDebugMsg(void);
    void updateDataFromCAN(void);
#endif
