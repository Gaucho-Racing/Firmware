#include <stdbool.h>
#include <string.h>

#include "CubeCAN_Config.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Logomatic.h"
#include "Private/PrivateCubeMXCAN.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim != NULL && htim->Instance == CUBEMX_CAN_TIMER_INSTANCE) {
		CubeMXCan_Tick();
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	(void)RxFifo0ITs;
	CubeMXCan_OnRxFifo0(hfdcan);
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
	(void)hfdcan;
	(void)BufferIndexes;
}

void CubeMXCan_Private_DispatchRx(FDCAN_HandleTypeDef *hfdcan)
{
	CubeMXCan_OnRxFifo0(hfdcan);
}

void CubeMXCan_OnRxFifo0(FDCAN_HandleTypeDef *hfdcan)
{
	if (hfdcan == NULL) {
		return;
	}

	CubeCAN_RxCallback rx_callback_stack = NULL;
	void *user_ctx_stack = NULL;

	CRITICAL_SECTION {
		CubeMXCan_Handle *handle = CubeMXCan_Private_GetHandle(hfdcan);
		if (handle != NULL && handle->config.rx_callback != NULL) {
			rx_callback_stack = handle->config.rx_callback;
			user_ctx_stack = handle->config.user_ctx;
		}
	}

	if (rx_callback_stack == NULL) {
		FDCAN_RxHeaderTypeDef rx_header = {0};
		uint8_t rx_data[FDCAN_MAX_DATA_BYTES] = {0};
		(void)HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);
		LOGOMATIC("CubeMXCan_OnRxFifo0: no handle or callback registered for %s\n", CAN_GetInstanceName(hfdcan->Instance));
		return;
	}

	FDCAN_RxHeaderTypeDef rx_header = {0};
	uint8_t rx_data[FDCAN_MAX_DATA_BYTES] = {0};
	if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) != HAL_OK) {
		LOGOMATIC("CubeMXCan_OnRxFifo0: failed to get RX message\n");
		return;
	}

	CAN_Identifier identifier = Deconstruct_CAN_Identifier(rx_header.Identifier);
	uint8_t size = CubeMXCan_Private_DlcToBytes(rx_header.DataLength);
	rx_callback_stack(user_ctx_stack, &identifier, rx_data, size);
}
