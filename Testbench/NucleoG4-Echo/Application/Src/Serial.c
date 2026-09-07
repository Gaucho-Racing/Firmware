#include "Serial.h"

#include <stdint.h>

#include "Logomatic.h"

void Rx_Callback(const uint8_t *const buffer, const uint16_t length)
{
	LOGOMATIC_INFO("%.*s", length, buffer);
	CubeVCP_SendString((const char *const)buffer, length);
}
