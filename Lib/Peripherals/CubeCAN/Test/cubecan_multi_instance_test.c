#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

int main(void)
{
	FDCAN_HandleTypeDef cans[CUBEMX_CAN_MAX_INSTANCES] = {
	    {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}},
	    {.Init = {.FrameFormat = FDCAN_FRAME_FD_NO_BRS}},
	    {.Init = {.FrameFormat = FDCAN_FRAME_FD_BRS}},
	};
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = 1U};
	CubeCAN_Handle *handles[CUBEMX_CAN_MAX_INSTANCES];
	uint8_t payload = 0xA5U;

	CubeCAN_TestHal_Reset();
	for (uint8_t index = 0U; index < CUBEMX_CAN_MAX_INSTANCES; ++index) {
		handles[index] = CubeCAN_Entrance(&cans[index], &config);
		assert(handles[index] != NULL && "Each configured instance should initialize");
		assert(CubeCAN_Send(handles[index], 2U, index, &payload, sizeof(payload)) == HAL_OK && "Each instance should queue a message");
	}
	FDCAN_HandleTypeDef extra_can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	assert(CubeCAN_Entrance(&extra_can, &config) == NULL && "Entrance should fail when all CAN slots are occupied");

	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_count == CUBEMX_CAN_MAX_INSTANCES && "One tick should service every active can instance");
	for (uint8_t index = 0U; index < CUBEMX_CAN_MAX_INSTANCES; ++index) {
		assert(cubecan_test_hal.tx_handles[index] == &cans[index] && "Each tx message should use its owning can instance");
		assert(CubeCAN_Exit(handles[index]) == HAL_OK && "Each can instance should exit cleanly");
	}
	return 0;
}
