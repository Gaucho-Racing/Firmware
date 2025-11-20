#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t data = 0;

	data = SetBitsInByte(data, 0, 3, 0x3);
	if (GETBITS(data, 0, 3) != 0x3) {
		return 1;
	}
	data = SetBitsInByte(data, 3, 2, 0x2);
	if (GETBITS(data, 3, 2) != 0x2) {
		return 2;
	}
	data = SetBitsInByte(data, 5, 3, 0x1);
	if (GETBITS(data, 5, 3) != 0x1) {
		return 3;
	}
	if (GETBITS(data, 0, 3) != 0x3) {
		return 4;
	}
	if (GETBITS(data, 3, 2) != 0x2) {
		return 5;
	}
	if (GETBITS(data, 5, 3) != 0x1) {
		return 6;
	}

	return 0;
}
