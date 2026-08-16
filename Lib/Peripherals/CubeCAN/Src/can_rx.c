#include <stdatomic.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeCAN.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "Unused.h"
#include "main.h"

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) {
		LOGOMATIC_ERROR("CAN Rx FIFO is overflowing, messages are being lost!\n");
	}

	if (hfdcan == NULL) {
		return;
	}

	CubeCAN_Config_Context context;
	CubeCAN_RxCallback rx_callback = NULL;
	bool is_started = false;

	CRITICAL_SECTION
	{
		for (uint8_t i = 0U; i < CUBEMX_CAN_MAX_INSTANCES; ++i) {
			if (handles[i].hfdcan == hfdcan) {
				context = handles[i].config.context;
				rx_callback = handles[i].config.rx_callback;
				is_started = handles[i].started;
				break;
			}
		}
	}

	if (!is_started || rx_callback == NULL) {
		FDCAN_RxHeaderTypeDef dump_header;
		uint8_t dump_data[FDCAN_MAX_DATA_BYTES];

		while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0U) {
			(void)HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &dump_header, dump_data);
		}

		LOGOMATIC_WARNING("HAL_FDCAN_RxFifo0Callback: Message dropped, unmapped handle or instance stopped\n");
		return;
	}

	FDCAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[FDCAN_MAX_DATA_BYTES];

	while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0U) {
		if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK) {
			CAN_Identifier id = CubeCAN_Deconstruct_Identifier(rx_header.Identifier);
			uint8_t size = CubeCAN_Private_DlcToBytes(rx_header.DataLength);
			rx_callback(&context, &id, rx_data, size);
		} else {
			LOGOMATIC_ERROR("HAL_FDCAN_RxFifo0Callback: Failed to get Rx message from FIFO\n");
			break;
		}
	}
}
