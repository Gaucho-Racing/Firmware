#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "CubeCAN.h"
#include "cubecan_test_hal.h"

static CubeCAN_Handle *start_can(FDCAN_HandleTypeDef *can)
{
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = 7U};
	return CubeCAN_Entrance(can, &config);
}

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_FD_BRS}};
	uint8_t payload[12];
	for (uint8_t index = 0U; index < sizeof(payload); ++index) {
		payload[index] = index + 1U;
	}

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = start_can(&can);
	assert(handle != NULL && "Entrance should return a handle");
	assert(CubeCAN_Send(handle, 3U, 0x23U, payload, sizeof(payload)) == HAL_OK && "Valid payload should queue");
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_count == 1U && "1 queued message should transmit");
	assert(cubecan_test_hal.tx_headers[0].Identifier == ((7U << 20U) | (0x23U << 8U) | 3U) && "Tx identifier should encode both nodes and message");
	assert(cubecan_test_hal.tx_headers[0].DataLength == FDCAN_DLC_BYTES_12 && "Tx dlc should encode twelve bytes");
	assert(cubecan_test_hal.tx_headers[0].FDFormat == FDCAN_FD_CAN && "FD frame format should be selected");
	assert(cubecan_test_hal.tx_headers[0].BitRateSwitch == FDCAN_BRS_ON && "BRS should be enabled");
	assert(memcmp(cubecan_test_hal.tx_data[0], payload, sizeof(payload)) == 0 && "Tx payload should be preserved");

	can.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	assert(CubeCAN_Send(handle, 3U, 0x23U, payload, sizeof(payload)) == HAL_OK && "FD without bit-rate switching should queue");
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_headers[1].FDFormat == FDCAN_FD_CAN && "FD without BRS should use FD format");
	assert(cubecan_test_hal.tx_headers[1].BitRateSwitch == FDCAN_BRS_OFF && "FD without BRS should disable bit-rate switching");

	assert(CubeCAN_Send(handle, 3U, 1U, payload, 65U) == HAL_ERROR && "Payloads over 64B should fail");
	assert(CubeCAN_Send(NULL, 3U, 1U, payload, 1U) == HAL_ERROR && "Null handles should fail");
	assert(CubeCAN_Send(handle, 3U, 1U, NULL, 1U) == HAL_ERROR && "Null payloads should fail");
	can.Init.FrameFormat = 0xdeadU;
	assert(CubeCAN_Send(handle, 3U, 1U, payload, 1U) == HAL_ERROR && "Unsupported frame formats should fail");
	can.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	for (uint8_t index = 0U; index < CUBEMX_CAN_TX_QUEUE_SIZE; ++index) {
		payload[0] = index;
		assert(CubeCAN_Send(handle, 3U, 2U, payload, 1U) == HAL_OK && "Messages should queue while capacity remains");
	}
	payload[0] = 99U;
	assert(CubeCAN_Send(handle, 3U, 2U, payload, 1U) == HAL_BUSY && "Full queues should report a dropped message");
	CubeCAN_TestHal_AdvanceTick();
	CubeCAN_Tick();
	assert(cubecan_test_hal.tx_data[2][0] == 1U && "Queued messages should transmit fifo order");
	assert(cubecan_test_hal.tx_headers[2].FDFormat == FDCAN_CLASSIC_CAN && "Classic frames should select classic can");

	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	return 0;
}
