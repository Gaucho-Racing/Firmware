#include <stdint.h>
#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

#define TICK_FREQ 1000

#ifdef LOGOMATIC_ENABLED
    #define LOGOMATIC(...) printf(__VA_ARGS__)
#else
    #define LOGOMATIC(...) UNUSED(NULL)
#endif

uint32_t millis(void);
uint8_t getBit(uint8_t number, uint8_t indexFromLeft);
uint8_t getBits(uint8_t number, uint8_t indexFromLeft, uint8_t length);

#endif
