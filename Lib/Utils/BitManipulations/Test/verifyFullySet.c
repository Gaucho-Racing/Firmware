#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t testByte = 0;
	for (int i = 0; i < 8; i++) {
		if (GetBitInByte(testByte, i) != false) {
			return 1;
		}
	}

	testByte = 0xFF;
	for (int i = 0; i < 8; i++) {
		if (GetBitInByte(testByte, i) != true) {
			return 2;
		}
	}

	uint32_t testInt = 0;
	for (int i = 0; i < 32; i++) {
		if (GetBitInByteArray((uint8_t *)&testInt, i) != false) {
			return 3;
		}
	}

	testInt = 0xFFFFFFFF;
	for (int i = 0; i < 32; i++) {
		if (GetBitInByteArray((uint8_t *)&testInt, i) != true) {
			return 4;
		}
	}

	uint8_t test = 0;
	for (int i = 0; i < UINT8_MAX; i++) {
		if (GetBitsInByte(test, 0, 8) != test) {
			return 5;
		}
		test++;
	}

	test = 0;
	for (int i = 0; i < UINT8_MAX; i++) {
		if (GetBitsInByte(test, 0, 4) != (test & 0x0F)) {
			return 6;
		}
		test++;
	}

	test = 0;
	for (int i = 0; i < UINT8_MAX; i++) {
		if (GetBitsInByte(test, 4, 4) != ((test >> 4) & 0x0F)) {
			return 7;
		}
		test++;
	}

	return 0;
}
