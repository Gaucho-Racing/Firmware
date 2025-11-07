#include <stdint.h>
#include <stdbool.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t test = 1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            bool expected = (j == i) ? true : false;
            if (GetBitInByte(test, j) != expected) {
                return 1;
            }
        }
        test <<= 1;
    }

    uint16_t testInt = 1;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            bool expected = (j == i) ? true : false;
            if (GetBitInByteArray((uint8_t*)&testInt, j) != expected) {
                return 2;
            }
        }
        testInt <<= 1;
    }

    uint32_t testLong = 1;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            bool expected = (j == i) ? true : false;
            if (GetBitInByteArray((uint8_t*)&testLong, j) != expected) {
                return 3;
            }
        }
        testLong <<= 1;
    }

    uint64_t testLongLong = 1;
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            bool expected = (j == i) ? true : false;
            if (GetBitInByteArray((uint8_t*)&testLongLong, j) != expected) {
                return 4;
            }
        }
        testLongLong <<= 1;
    }

    return 0;
}
