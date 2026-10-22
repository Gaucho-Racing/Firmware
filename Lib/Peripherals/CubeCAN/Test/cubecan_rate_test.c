#include <assert.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

int main(void)
{
	CubeCAN_TestHal_Reset();
	for (uint8_t index = 0U; index < 11U; ++index) {
		CubeCAN_TestHal_AdvanceTick();
	}

	CubeCAN_Tick();
	CubeCAN_Tick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.error_handler_count == 0U && "Three ticks in 1 ms should be allowed");
	CubeCAN_Tick();
	assert(cubecan_test_hal.error_handler_count == 1U && "The fourth rapid tick should invoke the error handler");
	return 0;
}
