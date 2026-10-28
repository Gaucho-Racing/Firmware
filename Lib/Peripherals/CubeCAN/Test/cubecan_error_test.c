#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = 1U};
	uint8_t payload = 0xabu;

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "Entrance should return a handle");
	assert(CubeCAN_Send(handle, 2U, 1U, &payload, 1U) == HAL_OK && "Error-path payload should queue");
	cubecan_test_hal.tx_fifo_free_level = 0U;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_count == 0U && "Full tx FIFO should prevent submission");
	cubecan_test_hal.tx_fifo_free_level = 1U;
	cubecan_test_hal.tx_status = HAL_ERROR;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_count == 1U && "HAL tx failure should still record the attempt");
	cubecan_test_hal.tx_status = HAL_OK;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_count == 2U && "Queued message should retry on the next tick");
	assert(cubecan_test_hal.tx_data[1][0] == payload && "Retried tx should preserve its payload");

	can.ProtocolStatus.BusOff = 1U;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.stop_count == 1U && "Bus-off recovery should stop the peripheral");
	assert(cubecan_test_hal.init_count == 1U && "Bus-off recovery should reinitialize the peripheral");
	assert(cubecan_test_hal.activate_count == 2U && "Bus-off recovery should restore notifications");
	assert(cubecan_test_hal.start_count == 2U && "Bus-off recovery should restart the peripheral");

	can.ProtocolStatus.BusOff = 0U;
	can.State = HAL_FDCAN_STATE_READY;
	cubecan_test_hal.restricted_operation = true;
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.restricted_exit_count == 1U && "Restricted operation should be exited");
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	return 0;
}
