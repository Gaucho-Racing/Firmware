#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "bitManipulations.h"

int main(void)
{
    uint8_t test = -1;
    bool expectedBits[8] = {true, true, true, true, true, true, true, true};
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 1;
        }
    }

    test = 0;
    memcpy(expectedBits, (bool[]){false, false, false, false, false, false, false, false}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 2;
        }
    }

    test = 0xAA; // 0b10101010
    memcpy(expectedBits, (bool[]){false, true, false, true, false, true, false, true}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 3;
        }
    }

    test = 0x55; // 0b01010101
    memcpy(expectedBits, (bool[]){true, false, true, false, true, false, true, false}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 4;
        }
    }

    test = 0xF0; // 0b11110000
    memcpy(expectedBits, (bool[]){false, false, false, false, true, true, true, true}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 5;
        }
    }

    test = 0x0F; // 0b00001111
    memcpy(expectedBits, (bool[]){true, true, true, true, false, false, false, false}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 6;
        }
    }

    test = 0xC3; // 0b11000011
    memcpy(expectedBits, (bool[]){true, true, false, false, false, false, true, true}, sizeof(expectedBits));
    for (int i = 0; i < 8; i++) {
        if (GetBitInByte(test, i) != expectedBits[i]) {
            return 7;
        }
    }

    return 0;
}
