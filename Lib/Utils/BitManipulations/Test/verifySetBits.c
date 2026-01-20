#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t data = 0x00;
	data = SetBitsInByte(data, 0, 3, 0x3);
	if (data != 0x60)
	{
		return 1;
	}
	data = SetBitsInByte(data, 3, 2, 0x2);
	if (data != 0x70)
	{
		return 2;
	}
	data = SetBitsInByte(data, 5, 3, 0x1);
	if (data != 0x71)
	{
		return 3;
	}

	data = 0xFF;
	data = SetBitsInByte(data, 0, 4, 0x0);
	if (data != 0x0F)
	{
		return 4;
	}
	data = SetBitsInByte(data, 4, 4, 0x5);
	if (data != 0x05)
	{
		return 5;
	}
	data = SetBitsInByte(data, 2, 3, 0x2);
	if (data != 0x15)
	{
		return 6;
	}
	data = SetBitsInByte(data, 0, 2, 0x3);
	if (data != 0xD5)
	{
		return 7;
	}

	return 0;
}
