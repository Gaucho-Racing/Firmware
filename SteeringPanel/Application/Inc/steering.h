#include <stdint.h>

#include "CANdler.h"
#include "lvgl/lvgl.h"

#ifndef STEERING_H
    #define STEERING_H

    #define CELL_COUNT 140

    typedef struct {
        uint8_t cellVoltage;
        uint8_t cellTemperature;
    } IncomingACUCellData; // Do not reorder!!!

    typedef struct {
        IncomingACUCellData cellData[160];
        char debugMessage[64];              // Treat as a string, if first bit '\0' then no message, clear in steering
        uint8_t ecuPingMap[3];
        SteeringConfigMsg steeringConfigMsg;
    } IncomingData;

    typedef struct {
        SteeringStatusMsg steeringStatusMsg;
        // Add more if needed
    } OutgoingData;

    extern volatile IncomingData incomingData;
    extern volatile OutgoingData outgoingData;
    extern volatile int isDataChanged;

    void updatedDataRecieved(void);
#endif
