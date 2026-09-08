#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "Unused.h"
#include "cubecan_test_hal.h"

static uint32_t callback_count;

static void receive_callback(const CubeCAN_Config_Context *context, const CAN_Identifier *identifier, const uint8_t *data, uint8_t size)
{
	UNUSED(context);
	UNUSED(identifier);
	UNUSED(data);
	UNUSED(size);
	++callback_count;
}

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = receive_callback, .sending_node_id = GRCAN_Debugger};
	FDCAN_RxHeaderTypeDef header = {.Identifier = 1U, .DataLength = FDCAN_DLC_BYTES_1};
	uint8_t data = 0x5AU;

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "CAN entrance should return a handle");
	CubeCAN_TestHal_QueueRx(&header, &data, sizeof(data));
	cubecan_test_hal.rx_status = HAL_ERROR;
	HAL_FDCAN_RxFifo0Callback(&can, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
	assert(cubecan_test_hal.rx_count == 1U && "Failed RX reads should consume one attempted message");
	assert(callback_count == 0U && "Failed RX reads should not invoke the callback");
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	return 0;
}
