#include <stdint.h>
#include <stdbool.h>

#ifndef UTILS_H
    #define UTILS_H

    #ifdef LOGOMATIC_ENABLED
        #define LOGOMATIC(...) printf(__VA_ARGS__)
    #else
        #define LOGOMATIC(...) UNUSED(NULL)
    #endif

    typedef enum {
        PrimaryBusCAN = (uint8_t)0,
        DataBusCAN = (uint8_t)1,
        ChargingBusCAN = (uint8_t)2
    } BusCAN;

    // LSB is indexed 0
    bool getBit(uint8_t number, uint8_t index);

    // LSB is indexed 0
    uint8_t getBits(uint8_t number, uint8_t index, uint8_t length);
#endif
