#include <stdint.h>
#include <stdbool.h>

#ifndef CANDLER_H
    #define CANDLER_H

    #define LOCAL_GR_ID GR_STEERING_WHEEL

    #define TEST_NO_CAN

    extern volatile int numberOfBadMessages;

    void handleCANMessage(uint16_t msgID, uint8_t srcID, uint8_t *data, uint32_t length);

    typedef struct {
        uint8_t CMEandTME;
        uint8_t RMEandButtonMap;
    } SteeringStatusMsg;

    // EMTPY FOR NOW
    typedef struct {
        bool DONOTUSE;
    } SteeringConfigMsg;
#endif
