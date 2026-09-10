#include <assert.h>
#include <stddef.h>

#include "CubeCAN.h"
#include "PrivateInc/internal.h"
#include "cubecan_test_hal.h"

int main(void)
{
	FDCAN_HandleTypeDef can = {.Init = {.FrameFormat = FDCAN_FRAME_FD_BRS}};
	CubeCAN_Config config = {.context.busid_user_context = 0U, .rx_callback = NULL, .sending_node_id = GRCAN_Debugger};
	const uint8_t sizes[] = {0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 12U, 16U, 20U, 24U, 32U, 48U, 64U};
	uint8_t payload[64] = {0U};

	CubeCAN_TestHal_Reset();
	CubeCAN_Handle *handle = CubeCAN_Entrance(&can, &config);
	assert(handle != NULL && "CAN entrance should return a handle");
	for (uint8_t index = 0U; index < sizeof(sizes); ++index) {
		for (uint8_t byte = 0U; byte < sizes[index]; ++byte) {
			payload[byte] = (uint8_t)(index + byte);
		}
		assert(CubeCAN_Send(handle, GRCAN_CCU, GRCAN_PING, payload, sizes[index]) == HAL_OK && "Every valid dlc size should queue through the public api");
		CubeCAN_TestHal_AdvanceTick();
		CubeCAN_Tick();
		assert(cubecan_test_hal.tx_headers[index].DataLength == CubeCAN_Private_BytesToDlc(sizes[index]) && "Public tx should encode each valid size with the matching dlc");
		assert(cubecan_test_hal.tx_data[index][0] == payload[0] && "Public tx should preserve each payload");
	}
	assert(CubeCAN_Exit(handle) == HAL_OK && "Exit should succeed");
	return 0;
}
