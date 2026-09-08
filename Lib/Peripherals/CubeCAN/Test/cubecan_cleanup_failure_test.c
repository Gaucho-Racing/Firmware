#include <assert.h>

#include "CubeCAN.h"
#include "PrivateInc/internal.h"
#include "cubecan_test_hal.h"

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = GRCAN_Debugger};

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "Entrance should return a handle");
	cubecan_test_hal.stop_status = HAL_ERROR;
	assert(CubeCAN_Exit(handle) == HAL_ERROR && "Stop failure should fail exit");
	cubecan_test_hal.stop_status = HAL_OK;
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should recover after a stop failure");

	CubeCAN_TestHal_Reset();
	handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "Entrance should return a handle");
	cubecan_test_hal.deactivate_status = HAL_ERROR;
	assert(CubeCAN_Exit(handle) == HAL_ERROR && "Notification deactivation failure should fail exit");
	assert(CubeCAN_Entrance(&can, &config) == NULL && "Failed release should retain the mapped handle");
	cubecan_test_hal.deactivate_status = HAL_OK;
	assert(CubeCAN_Private_Release(handle) == HAL_OK && "Release should succeed after deactivation recovers");
	return 0;
}
