#include <stdint.h>

#include "bitManipulations.h"

int main(void)
{
	uint8_t data;

	for (int i = 0; i < 8; i++) {
		data = 1 << i;
		if (GETBIT(data, i) != 0) {
			return 1;
		}
	}

	for (int i = 0; i < 8; i++) {
		data = 0xFF ^ (1 << i);
		if (GETBIT(data, i) != 1) {
			return 2;
		}
	}

	return 0;
}
