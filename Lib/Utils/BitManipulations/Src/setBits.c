#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

void SetBitInByte(uint8_t *byte, uint8_t index, bool value)
{
    if (value) {
        *byte |= (1 << index);
    } else {
        *byte &= ~(1 << index);
    }
}

void SetBitsInByte(uint8_t *byte, uint8_t index, uint8_t length, uint8_t value)
{
    uint8_t mask = ((1 << length) - 1) << index;
    *byte = (*byte & ~mask) | ((value << index) & mask);
}

void SetBitInByteArray(uint8_t *byteArray, uint8_t bitIndex, bool value)
{
    SetBitInByte(&byteArray[bitIndex / 8], bitIndex % 8, value);
}
