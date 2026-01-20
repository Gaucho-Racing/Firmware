#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t data = 0x00;
	for (int i = 0; i < 8; i++) {
		if (GETBIT(data, i) != 0) {
			return 1;
		}
	}
	data = 0xFF;
	for (int i = 0; i < 8; i++) {
		if (GETBIT(data, i) != 1) {
			return 2;
		}
	}

	data = 0xAA;
	if (GETBIT(data, 0) != 1) {
		return 3;
	}
	if (GETBIT(data, 1) != 0) {
		return 4;
	}
	if (GETBIT(data, 2) != 1) {
		return 5;
	}
	if (GETBIT(data, 3) != 0) {
		return 6;
	}
	if (GETBIT(data, 4) != 1) {
		return 7;
	}
	if (GETBIT(data, 5) != 0) {
		return 8;
	}
	if (GETBIT(data, 6) != 1) {
		return 9;
	}
	if (GETBIT(data, 7) != 0) {
		return 10;
	}

	data = 0x01;
	if (GETBIT(data, 0) != 0) {
		return 11;
	}
	if (GETBIT(data, 1) != 0) {
		return 12;
	}
	if (GETBIT(data, 2) != 0) {
		return 13;
	}
	if (GETBIT(data, 3) != 0) {
		return 14;
	}
	if (GETBIT(data, 4) != 0) {
		return 15;
	}
	if (GETBIT(data, 5) != 0) {
		return 16;
	}
	if (GETBIT(data, 6) != 0) {
		return 17;
	}
	if (GETBIT(data, 7) != 1) {
		return 18;
	}

	data = 0x80;
	if (GETBIT(data, 0) != 1) {
		return 19;
	}
	if (GETBIT(data, 1) != 0) {
		return 20;
	}
	if (GETBIT(data, 2) != 0) {
		return 21;
	}
	if (GETBIT(data, 3) != 0) {
		return 22;
	}
	if (GETBIT(data, 4) != 0) {
		return 23;
	}
	if (GETBIT(data, 5) != 0) {
		return 24;
	}
	if (GETBIT(data, 6) != 0) {
		return 25;
	}
	if (GETBIT(data, 7) != 0) {
		return 26;
	}

	uint64_t bigData = 0xF0F0F0F0F0F0F0F0;
	if (GETBIT(bigData, 0) != 1) {
		return 27;
	}
	if (GETBIT(bigData, 4) != 0) {
		return 28;
	}
	if (GETBIT(bigData, 60) != 0) {
		return 29;
	}
	if (GETBIT(bigData, 63) != 0) {
		return 30;
	}

	return 0;
}
