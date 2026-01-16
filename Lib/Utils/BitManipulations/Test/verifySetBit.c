#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t data = 0x00;
	data = SetBitInByte(data, 0, true);
	if (data != 0x80) {
		return 1;
	}
	data = SetBitInByte(data, 7, true);
	if (data != 0x81) {
		return 2;
	}
	data = SetBitInByte(data, 0, false);
	if (data != 0x01) {
		return 3;
	}
	data = SetBitInByte(data, 7, false);
	if (data != 0x00) {
		return 4;
	}

	data = 0xFF;
	data = SetBitInByte(data, 3, false);
	if (data != 0xEF) {
		return 5;
	}
	data = SetBitInByte(data, 0, false);
	if (data != 0x6F) {
		return 6;
	}
	data = SetBitInByte(data, 7, true);
	if (data != 0x6F) {
		return 7;
	}
	data = SetBitInByte(data, 4, false);
	if (data != 0x67) {
		return 8;
	}

	return 0;
}
