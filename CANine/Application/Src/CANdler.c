#include "CANdler.h"

#include <stdint.h>

#include "Logomatic.h"

void LoopbackTest(uint32_t ID, void *data, uint32_t size)
{
	uint8_t *bytes = (uint8_t *)data;
	LOGOMATIC("Received CAN message with ID: 0x%lX, data: 0x", ID);
	for (uint32_t i = 0; i < size; i++) {
		LOGOMATIC("%02X", bytes[i]);
	}
	LOGOMATIC("\n");
}
