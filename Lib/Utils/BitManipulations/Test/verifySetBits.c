#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t test = 0;
	SetBitsInByte(&test, 0, 8, 0xFF);
	if (test != 0xFF) {
		return 1;
	}
	if (GetBitsInByte(test, 0, 8) != 0xFF) {
		return 2;
	}

	test = 0xFF;
	SetBitsInByte(&test, 0, 4, 0x0);
	if (test != 0xF0) {
		return 3;
	}
	if (GetBitsInByte(test, 0, 4) != 0x0) {
		return 4;
	}

	test = 0xFF;
	SetBitsInByte(&test, 4, 4, 0x0);
	if (test != 0x0F) {
		return 5;
	}
	if (GetBitsInByte(test, 4, 4) != 0x0) {
		return 6;
	}

	test = 0x00;
	SetBitsInByte(&test, 0, 8, 0xAA);
	if (test != 0xAA) {
		return 7;
	}
	if (GetBitsInByte(test, 0, 8) != 0xAA) {
		return 8;
	}

	test = 0x00;
	SetBitsInByte(&test, 2, 4, 0x0F);
	if (test != 0x3C) {
		return 9;
	}
	if (GetBitsInByte(test, 2, 4) != 0x0F) {
		return 10;
	}

	test = 0xFF;
	SetBitsInByte(&test, 3, 3, 0x0);
	if (test != 0xC7) {
		return 11;
	}
	if (GetBitsInByte(test, 3, 3) != 0x0) {
		return 12;
	}

	test = 0x00;
	SetBitsInByte(&test, 1, 5, 0x1F);
	if (test != 0x3E) {
		return 13;
	}
	if (GetBitsInByte(test, 1, 5) != 0x1F) {
		return 14;
	}

	test = 0xFF;
	SetBitsInByte(&test, 0, 3, 0x0);
	if (test != 0xF8) {
		return 15;
	}
	if (GetBitsInByte(test, 0, 3) != 0x0) {
		return 16;
	}

	test = 0x00;
	SetBitsInByte(&test, 5, 3, 0x07);
	if (test != 0xE0) {
		return 17;
	}
	if (GetBitsInByte(test, 5, 3) != 0x07) {
		return 18;
	}

	test = 0xFF;
	SetBitsInByte(&test, 4, 4, 0x0);
	if (test != 0x0F) {
		return 19;
	}
	if (GetBitsInByte(test, 4, 4) != 0x0) {
		return 20;
	}

	return 0;
}
