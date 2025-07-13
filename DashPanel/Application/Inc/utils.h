#include <stdint.h>
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H
    #ifdef LOGOMATIC_ENABLED
        #define LOGOMATIC(...) printf(__VA_ARGS__)
    #else
        #define LOGOMATIC(...) UNUSED(NULL)
    #endif

    uint8_t getBit(uint8_t number, uint8_t index);
    uint8_t getBits(uint8_t number, uint8_t index, uint8_t length);
    uint8_t findTernaryMax(const uint8_t a, const uint8_t b, const uint8_t c);
#endif
