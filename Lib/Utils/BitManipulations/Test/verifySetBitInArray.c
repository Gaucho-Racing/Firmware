#include <stdint.h>
#include <stdbool.h>

#include "bitManipulations.h"

int main(void)
{
    uint32_t testInt = 0;
    SetBitInByteArray((uint8_t*)&testInt, 0, true);
    if (testInt != 0x00000001) {
        return 1;
    }
    if (!GetBitInByteArray((uint8_t*)&testInt, 0)) {
        return 2;
    }

    testInt = 1;
    SetBitInByteArray((uint8_t*)&testInt, 31, true);
    if (testInt != 0x80000001) {
        return 3;
    }
    if (!GetBitInByteArray((uint8_t*)&testInt, 31)) {
        return 4;
    }

    testInt = 0xFFFFFFFF;
    SetBitInByteArray((uint8_t*)&testInt, 15, false);
    if (testInt != 0xFFFF7FFF) {
        return 5;
    }
    if (GetBitInByteArray((uint8_t*)&testInt, 15)) {
        return 6;
    }

    testInt = 0xFFFFFFFF;
    SetBitInByteArray((uint8_t*)&testInt, 0, false);
    if (testInt != 0xFFFFFFFE) {
        return 7;
    }
    if (GetBitInByteArray((uint8_t*)&testInt, 0)) {
        return 8;
    }

    testInt = 0xFFFFFFFF;
    SetBitInByteArray((uint8_t*)&testInt, 31, false);
    if (testInt != 0x7FFFFFFF) {
        return 9;
    }
    if (GetBitInByteArray((uint8_t*)&testInt, 31)) {
        return 10;
    }

    testInt = 0;
    for (int i = 0; i < 32; i++) {
        SetBitInByteArray((uint8_t*)&testInt, i, true);
        if (!GetBitInByteArray((uint8_t*)&testInt, i)) {
            return 11 + i;
        }
    }

    testInt = 0xFFFFFFFF;
    for (int i = 0; i < 32; i++) {
        SetBitInByteArray((uint8_t*)&testInt, i, false);
        if (GetBitInByteArray((uint8_t*)&testInt, i)) {
            return 43 + i;
        }
    }

    return 0;
}
