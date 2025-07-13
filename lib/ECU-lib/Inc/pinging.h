#include <stdint.h>

#include "main.h"

#ifndef PINGING_H
    #define PINGING_H

    #define PINGTIMEOUT 250 // Timeout time for pings in ms

    #ifdef ENABLE_THREE_MOTORS
    #define PINGCOUNT 9
    #else
    #define PINGCOUNT 7
    #endif

    extern const uint8_t pingIDs[PINGCOUNT]; // GR IDs of devices that are pinged
    extern uint32_t pingTimes[PINGCOUNT]; // round-trip ping times of said devices

    /**
    Sends a ping if there is a need to, manages missing ping responses.
    
    Can call as often as desired, will quite if nothing needed.
    */
    void pingSchedule(void);

    /**
    Handles ping responses and updates internal setup.
    Not dissimilar to CANDler operation but specific to pings.

    @param destID Desintation GR ID
    @param timestamp When the interrupt was recieved
    */
    void respondToPing(uint8_t destID, uint32_t timestamp);

#endif // PINGING_H
