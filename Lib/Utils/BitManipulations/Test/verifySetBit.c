#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
    uint8_t test = 0;
    SetBitInByte(&test, 0, true);
    if (test != 0x01) {
        return 1;
    }
    if (!GetBitInByte(test, 0)) {
        return 2;
    }

    test = 0;
    SetBitInByte(&test, 1, true);
    if (test != 0x02) {
        return 3;
    }
    if (!GetBitInByte(test, 1)) {
        return 4;
    }

    test = 0;
    SetBitInByte(&test, 2, true);
    if (test != 0x04) {
        return 5;
    }
    if (!GetBitInByte(test, 2)) {
        return 6;
    }

    test = 0;
    SetBitInByte(&test, 3, true);
    if (test != 0x08) {
        return 7;
    }
    if (!GetBitInByte(test, 3)) {
        return 8;
    }

    test = 0;
    SetBitInByte(&test, 4, true);
    if (test != 0x10) {
        return 9;
    }
    if (!GetBitInByte(test, 4)) {
        return 10;
    }

    test = 0;
    SetBitInByte(&test, 5, true);
    if (test != 0x20) {
        return 11;
    }
    if (!GetBitInByte(test, 5)) {
        return 12;
    }

    test = 0;
    SetBitInByte(&test, 6, true);
    if (test != 0x40) {
        return 13;
    }
    if (!GetBitInByte(test, 6)) {
        return 14;
    }

    test = 0;
    SetBitInByte(&test, 7, true);
    if (test != 0x80) {
        return 15;
    }
    if (!GetBitInByte(test, 7)) {
        return 16;
    }

    test = 0xFF;
    SetBitInByte(&test, 0, false);
    if (test != 0xFE) {
        return 17;
    }
    if (GetBitInByte(test, 0)) {
        return 18;
    }

    test = 0xFF;
    SetBitInByte(&test, 1, false);
    if (test != 0xFD) {
        return 19;
    }
    if (GetBitInByte(test, 1)) {
        return 20;
    }

    test = 0xFF;
    SetBitInByte(&test, 2, false);
    if (test != 0xFB) {
        return 21;
    }
    if (GetBitInByte(test, 2)) {
        return 22;
    }

    test = 0xFF;
    SetBitInByte(&test, 3, false);
    if (test != 0xF7) {
        return 23;
    }
    if (GetBitInByte(test, 3)) {
        return 24;
    }

    test = 0xFF;
    SetBitInByte(&test, 4, false);
    if (test != 0xEF) {
        return 25;
    }
    if (GetBitInByte(test, 4)) {
        return 26;
    }

    test = 0xFF;
    SetBitInByte(&test, 5, false);
    if (test != 0xDF) {
        return 27;
    }
    if (GetBitInByte(test, 5)) {
        return 28;
    }

    test = 0xFF;
    SetBitInByte(&test, 6, false);
    if (test != 0xBF) {
        return 29;
    }
    if (GetBitInByte(test, 6)) {
        return 30;
    }

    test = 0xFF;
    SetBitInByte(&test, 7, false);
    if (test != 0x7F) {
        return 31;
    }
    if (GetBitInByte(test, 7)) {
        return 32;
    }

    return 0;
}
