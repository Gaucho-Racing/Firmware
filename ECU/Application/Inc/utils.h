#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "CANdler.h"

#ifndef UTILS_H
    #define UTILS_H

    #define TICK_FREQ 1000

    #ifdef LOGOMATIC_ENABLED
        #define LOGOMATIC(...) printf(__VA_ARGS__)
    #else
        #define LOGOMATIC(...) UNUSED(NULL)
    #endif

    /**
    Prints the content of `globalStatus` over debug
    */
    void printGlobalStatus();

    /**
    Use in place of HAL Ticks for timing.

    @returns Current time in milliseconds.
    */
    uint32_t millis(void);

    /**
    @param number Byte in question
    @param index What place bit to to start from
    @returns A byte that represents the bit (basically a bool).
    */
    bool getBit(uint8_t number, uint8_t index);

    /**
    Complicated... look at the implementation to be sure.

    @param number Byte in question
    @param indexFromLeft What place bit to start looking at from the right side
    @param length How many bits after the index to return
    @returns A byte that represents the bits in question.
    */
    uint8_t getBits(uint8_t number, uint8_t index, uint8_t length);

    /**
    Essentially a nuanced flip-flop of the ECU software latch.

    @param close Written to the ECU software latch

    Basically it should immediately start discharging no matter where it is
    */
    void setSoftwareLatch(bool close);

    /**
    Checks for BSE APPS violation
    Checks for apps violation given the throttles, throttle request, and brake signal
     */
    bool checkBSEAPPSviolation(uint16_t throttle1, uint16_t throttle2, float pedalTravel, float brake);
    
    /**
    Validate torque request to AMK motors and manage heat
     */
    void validateForwardTorqueRequest(uint16_t* tqr, volatile uint16_t* heatCapacity);


    /**
    Validate regen request (negative current) and manage the battery heat capacity
     */
    void validateRegenRequest(uint16_t* fwdTqr1, uint16_t* fwdTqr2, uint16_t* rTqr);

    /**
    Will tell you if the driver is actuallt pressing the brake
     */
    bool pressingBrake();

    /**
    Sends apps violation message
     */
    void sendBseAppsViolationMessage(void);

    /**
    Parses the message in question to see if there are any errors.

    @param acuMsgTwo ACU Status Message 2
    @return A byte representing the errorfulness.
    */
    bool ACUError(ACU_Status_MsgTwo *acuMsgTwo);

    /**
    Parses the message in question to see if there are any errors.

    @param msgGriThree GR Inverter Status Message 3
    @return A byte representing the errorfulness.
    */
    bool GRIError(Inverter_Status_Msg_Three *msgGriThree);

    /**
    Parses the message in question to see if there are any errors.

    @param acuMsgTwo ACU Status Message 2
    @return A byte representing the errorfulness.
    */
    bool ACUWarning(ACU_Status_MsgTwo *acuMsgTwo);

#endif // UTILS_H
