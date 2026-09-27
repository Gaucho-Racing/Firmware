#include <assert.h>
#include <string.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"
#include "main.h"

static uint32_t callback_count;
static CAN_Identifier callback_ids[2];
static uint8_t callback_data[2][64];
static uint8_t callback_sizes[2];
static uint32_t callback_context;

static void receive_callback(const CubeCAN_Config_Context *context, const CAN_Identifier *identifier, const uint8_t *data, uint8_t size)
{
	const uint32_t index = callback_count++;
	callback_context = context->busid_user_context;
	callback_ids[index] = *identifier;
	callback_sizes[index] = size;
	memcpy(callback_data[index], data, size);
}

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_Config config = {.context.busid_user_context = 42U, .rx_callback = receive_callback, .sending_node_id = 1U};
	uint8_t first_data[] = {1U, 2U, 3U};
	uint8_t second_data[] = {9U, 8U, 7U, 6U};
	FDCAN_RxHeaderTypeDef first_header = {.Identifier = (5U << 20U) | (0x22U << 8U) | 1U, .DataLength = FDCAN_DLC_BYTES_3};
	FDCAN_RxHeaderTypeDef second_header = {.Identifier = (6U << 20U) | (0x33U << 8U) | 2U, .DataLength = FDCAN_DLC_BYTES_4};

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "CAN entrance should return a handle");
	CubeCAN_TestHal_QueueRx(&first_header, first_data, sizeof(first_data));
	CubeCAN_TestHal_QueueRx(&second_header, second_data, sizeof(second_data));
	HAL_FDCAN_RxFifo0Callback(&can, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
	assert(callback_count == 2U && "Each queued rx message should invoke the callback");
	assert(callback_context == 42U && "Rx callback should receive its configured context");
	assert((callback_ids[0].tx_node_id == 5U && callback_ids[0].msg_id == 0x22U && callback_ids[0].rx_node_id == 1U) && "First rx identifier should be decoded");
	assert((callback_ids[1].tx_node_id == 6U && callback_ids[1].msg_id == 0x33U && callback_ids[1].rx_node_id == 2U) && "Second rx identifier should be decoded");
	assert((callback_sizes[0] == 3U && callback_sizes[1] == 4U) && "Rx sizes should match their DLCs");
	assert(memcmp(callback_data[0], first_data, 3U) == 0 && "First rx payload should be preserved");
	assert(memcmp(callback_data[1], second_data, 4U) == 0 && "Second rx payload should be preserved");
	assert(cubecan_test_hal.rx_count == 2U && "Both rx messages should be consumed");

	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	CubeCAN_TestHal_QueueRx(&first_header, first_data, sizeof(first_data));
	HAL_FDCAN_RxFifo0Callback(&can, FDCAN_IT_RX_FIFO0_MESSAGE_LOST);
	assert(cubecan_test_hal.rx_count == 3U && "Stopped handles should drain rx messages");
	assert(callback_count == 2U && "Stopped handles should not invoke the callback");
	FDCAN_HandleTypeDef unrelated_can = {.Init = {.FrameFormat = FDCAN_FRAME_CLASSIC}};
	CubeCAN_TestHal_QueueRx(&first_header, first_data, sizeof(first_data));
	HAL_FDCAN_RxFifo0Callback(&unrelated_can, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
	assert(cubecan_test_hal.rx_count == 4U && "Unmapped handles should drain rx messages");
	assert(callback_count == 2U && "Unmapped handles should not invoke the callback");
	return 0;
}
