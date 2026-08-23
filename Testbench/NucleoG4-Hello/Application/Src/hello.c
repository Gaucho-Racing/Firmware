#include "hello.h"

#include <stdint.h>

#include "main.h"
#include "Logomatic.h"

HAL_StatusTypeDef say_hello(uint8_t count)
{
	if (count == 0) {
		return HAL_ERROR;
	}

	for (uint8_t i = 0; i < count; i++) {
		LOGOMATIC_INFO("Hello, World!\n");
		HAL_Delay(1000);
	}
	return HAL_OK;
}
