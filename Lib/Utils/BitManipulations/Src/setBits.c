#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "bitManipulations.h"

uint8_t SetBitInByte(uint8_t number, uint8_t index, bool value)  {
	if (index > 7) {
		return number;	// Invalid index, undefined behavior
	}

	if (value) {
		return number | (1u << (7 - index));	// Set bit to 1
	} else {
		return number & ~(1u << (7 - index));	// Set bit to 0
	}
}

uint8_t SetBitsInByte(uint8_t number, uint8_t index, uint8_t length, uint8_t value)
{
	if (length == 0 || index + length > 8) {
		return number;	// Invalid parameters, undefined behavior
	}

	uint8_t mask = ((1u << length) - 1) << (sizeof(uint8_t) * 8 - index - length);
	return (number & ~mask) | ((value << (sizeof(uint8_t) * 8 - index - length)) & mask);
}
