#include <stdint.h>
#include <stdbool.h>

#ifndef UTILS_H
    #define UTILS_H

    // LSB is indexed 0
    bool getBit(uint8_t number, uint8_t index);

    // LSB is indexed 0
    uint8_t getBits(uint8_t number, uint8_t index, uint8_t length);
#endif
