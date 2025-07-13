#include <stdint.h>

#ifndef CANDLER_H
    #define CANDLER_H
    extern volatile uint8_t numberOfBadMessages;
    void handleCANMessage(uint16_t id, uint8_t srcID, uint8_t *data, uint32_t length);
#endif
