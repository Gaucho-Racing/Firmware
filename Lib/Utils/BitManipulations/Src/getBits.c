#include <stdbool.h>
#include <stdint.h>

#include "bitManipulations.h"

bool GetBitInByte(uint8_t number, uint8_t index)
{
	return GetBitsInByte(number, index,
			     1); // Equivalent to `(number >> index) & 0x1
}

uint8_t GetBitsInByte(uint8_t number, uint8_t index, uint8_t length)
{
	return (number >> index) & ((1 << length) - 1);
}

bool GetBitInByteArray(const uint8_t *byteArray, uint8_t bitIndex)
{
	return GetBitInByte(byteArray[bitIndex / 8], bitIndex % 8);
}
