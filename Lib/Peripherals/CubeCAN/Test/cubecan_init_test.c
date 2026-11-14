#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

static CubeCAN_Config test_config(void)
{
	return (CubeCAN_Config){.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = 1U};
}

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_Config config = test_config();
	CubeCAN_TestHal_Reset();
	assert(CubeCAN_Entrance(NULL, &config) == NULL && "Null CAN handles should fail entrance");
	assert(CubeCAN_Entrance(&can, NULL) == NULL && "Null configurations should fail entrance");

	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "Entrance should return a handle");
	assert(cubecan_test_hal.activate_count == 1U && "Notifications should activate once");
	assert(cubecan_test_hal.last_interrupts == (FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_MESSAGE_LOST) && "all RX notifications should be enabled");
	assert(cubecan_test_hal.start_count == 1U && "Should start once");
	assert(CubeCAN_Entrance(&can, &config) == NULL && "Duplicate entrance should fail");
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	assert(cubecan_test_hal.stop_count == 2U && "Exit should stop during cleanup");
	assert(cubecan_test_hal.deactivate_count == 1U && "Notifications should deactivate once");
	assert(CubeCAN_Exit(handle) == HAL_ERROR && "Reusing an exited handle should fail");

	CubeCAN_TestHal_Reset();
	can.State = HAL_FDCAN_STATE_RESET;
	cubecan_test_hal.activate_status = HAL_ERROR;
	assert(CubeCAN_Entrance(&can, &config) == NULL && "Notification activation failure should abort entrance");
	assert(cubecan_test_hal.start_count == 0U && "Start should not run after activation failure");

	CubeCAN_TestHal_Reset();
	cubecan_test_hal.start_status = HAL_ERROR;
	assert(CubeCAN_Entrance(&can, &config) == NULL && "Start failure should abort entrance");
	assert(cubecan_test_hal.stop_count == 1U && "Failed start should stop during cleanup");
	assert(cubecan_test_hal.deactivate_count == 1U && "Failed start should deactivate notifications");
	assert(CubeCAN_Exit(NULL) == HAL_ERROR && "Exiting a null handle should fail");
	return 0;
}
