#include <stdint.h>
#include <stdbool.h>

#include "CANdler.h"

#ifndef INVERTER_H
    #define INVERTER_H

    typedef struct {
        uint16_t acCurrent;
        uint16_t dcCurrent;
        uint16_t rpmLimit;
        uint8_t driveEnable;
    } InverterSettings;

    extern volatile InverterSettings globalInverterSettings[3]; // Set to 4 iff fourth motor/inverter

    extern volatile DTI_Data globalInverterData;    

    void controlInverters(bool driveEnable);
    void sendInverterCommand(void);

#endif // INVERTER_H



