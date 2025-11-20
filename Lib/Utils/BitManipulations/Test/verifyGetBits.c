#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
    uint8_t data = 0x00;
    for (int i = 0; i < 8; i++) {
        if (GETBITS(data, i, 1) != 0) {
            return 1;
        }
    }

    data = 0xFF;
    for (int i = 0; i < 8; i++) {
        if (GETBITS(data, i, 1) != 1) {
            return 2;
        }
    }

    data = 0xAA;
    if (GETBITS(data, 0, 1) != 1) {
        return 3;
    }
    if (GETBITS(data, 0, 2) != 0x2) {
        return 4;
    }
    if (GETBITS(data, 0, 3) != 0x5) {
        return 5;
    }
    if (GETBITS(data, 0, 4) != 0xA) {
        return 6;
    }
    if (GETBITS(data, 0, 5) != 0x15) {
        return 7;
    }
    if (GETBITS(data, 0, 6) != 0x2A) {
        return 8;
    }
    if (GETBITS(data, 0, 7) != 0x55) {
        return 9;
    }
    if (GETBITS(data, 0, 8) != 0xAA) {
        return 10;
    }

    data = 0x01;
    if (GETBITS(data, 0, 1) != 0) {
        return 11;
    }
    if (GETBITS(data, 0, 2) != 0) {
        return 12;
    }
    if (GETBITS(data, 1, 3) != 0) {
        return 13;
    }
    if (GETBITS(data, 2, 4) != 0) {
        return 14;
    }
    if (GETBITS(data, 3, 5) != 1) {
        return 15;
    }
    if (GETBITS(data, 4, 4) != 1) {
        return 16;
    }
    if (GETBITS(data, 5, 3) != 1) {
        return 17;
    }
    if (GETBITS(data, 6, 2) != 1) {
        return 18;
    }

    data = 0x80;
    if (GETBITS(data, 0, 1) != 1) {
        return 19;
    }
    if (GETBITS(data, 1, 1) != 0) {
        return 20;
    }
    if (GETBITS(data, 0, 2) != 2) {
        return 21;
    }
    if (GETBITS(data, 0, 3) != 4) {
        return 22;
    }
    if (GETBITS(data, 0, 4) != 8) {
        return 23;
    }
    if (GETBITS(data, 0, 5) != 16) {
        return 24;
    }
    if (GETBITS(data, 0, 6) != 32) {
        return 25;
    }
    if (GETBITS(data, 0, 7) != 64) {
        return 26;
    }
    if (GETBITS(data, 0, 8) != 128) {
        return 27;
    }

    uint32_t largeData = 0xF0F0F0F0;
    if (GETBITS(largeData, 0, 4) != 0xF) {
        return 28;
    }
    if (GETBITS(largeData, 4, 4) != 0x0) {
        return 29;
    }
    if (GETBITS(largeData, 8, 8) != 0xF0) {
        return 30;
    }
    if (GETBITS(largeData, 16, 8) != 0xF0) {
        return 31;
    }
    if (GETBITS(largeData, 24, 8) != 0xF0) {
        return 32;
    }
    if (GETBITS(largeData, 28, 4) != 0x0) {
        return 33;
    }
    if (GETBITS(largeData, 0, 32) != 0xF0F0F0F0) {
        return 34;
    }

     uint64_t bigData = 0xAAAAFFFF0000ABCD;
    if (GETBITS(bigData, 0, 16) != 0xAAAA) {
        return 35;
    }
    if (GETBITS(bigData, 16, 16) != 0xFFFF) {
        return 36;
    }
    if (GETBITS(bigData, 32, 16) != 0x0000) {
        return 37;
    }
    if (GETBITS(bigData, 48, 16) != 0xABCD) {
        return 38;
    }
    if (GETBITS(bigData, 8, 16) != 0xAAFF) {
        return 39;
    }

    return 0;
}
