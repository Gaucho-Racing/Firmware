#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

static CubeCAN_Handle *start_bus(FDCAN_HandleTypeDef *can)
{
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = GRCAN_Debugger};
	return CubeCAN_Entrance(can, &config);
}

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = start_bus(&can);
	assert(handle != NULL && "Entrance should return a handle");
	can.ProtocolStatus.BusOff = 1U;
	cubecan_test_hal.init_status = HAL_ERROR;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.init_count == 1U && "Bus-off recovery should attempt initialization");
	assert(cubecan_test_hal.activate_count == 1U && "Failed recovery initialization should not reactivate notifications");
	assert(cubecan_test_hal.start_count == 1U && "Failed recovery initialization should not restart the peripheral");
	can.ProtocolStatus.BusOff = 0U;
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should clean up after recovery failure");

	CubeCAN_TestHal_Reset();
	handle = start_bus(&can);
	assert(handle != NULL && "Instance should be reusable after recovery cleanup");
	can.ProtocolStatus.BusOff = 1U;
	cubecan_test_hal.activate_status = HAL_ERROR;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.init_count == 1U && "Recovery should initialize before notification activation");
	assert(cubecan_test_hal.activate_count == 2U && "Recovery should attempt notification activation");
	assert(cubecan_test_hal.start_count == 1U && "Failed notification activation should prevent restart");
	can.ProtocolStatus.BusOff = 0U;
	cubecan_test_hal.activate_status = HAL_OK;
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should clean up after notification failure");

	CubeCAN_TestHal_Reset();
	handle = start_bus(&can);
	assert(handle != NULL && "Instance should be reusable after notification failure");
	can.ProtocolStatus.BusOff = 1U;
	cubecan_test_hal.start_status = HAL_ERROR;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.init_count == 1U && "Recovery should initialize before restart");
	assert(cubecan_test_hal.activate_count == 2U && "Recovery should reactivate notifications before restart");
	assert(cubecan_test_hal.start_count == 2U && "Recovery should attempt a restart");
	can.ProtocolStatus.BusOff = 0U;
	cubecan_test_hal.start_status = HAL_OK;
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should clean up after restart failure");

	CubeCAN_TestHal_Reset();
	handle = start_bus(&can);
	assert(handle != NULL && "Instance should be reusable after restart failure");
	can.State = HAL_FDCAN_STATE_READY;
	cubecan_test_hal.restricted_operation = true;
	cubecan_test_hal.restricted_exit_status = HAL_ERROR;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.restricted_exit_count == 1U && "Restricted recovery should attempt to exit the mode");
	cubecan_test_hal.restricted_exit_status = HAL_OK;
	can.State = HAL_FDCAN_STATE_BUSY;
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should clean up after restricted-mode failure");
	return 0;
}
