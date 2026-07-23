#include <stdatomic.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"
#include "Unused.h"
#include "main.h"

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	UNUSED(RxFifo0ITs);

	if (hfdcan == NULL) {
		return;
	}

	CubeMXCan_Handle *handle = NULL;
	for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
		if (handles[i].hfdcan == hfdcan) {
			handle = &handles[i];
			break;
		}
	}

	if (handle == NULL || handle->config.rx_callback == NULL) {
		FDCAN_RxHeaderTypeDef rx_header;
		uint8_t rx_data[FDCAN_MAX_DATA_BYTES];
		(void)HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);
		LOGOMATIC("Rx Callback missing for instance\n");
		return;
	}

	FDCAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[FDCAN_MAX_DATA_BYTES];

	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
		CAN_Identifier id = Deconstruct_CAN_Identifier(rx_header.Identifier);
		uint8_t size = CubeMXCan_Private_DlcToBytes(rx_header.DataLength);
		handle->config.rx_callback(handle->config.user_ctx, &id, rx_data, size);
	}
}
