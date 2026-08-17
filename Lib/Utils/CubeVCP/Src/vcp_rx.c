#include <stdatomic.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeVCP.h"
#include "CubeVCP_Config.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"

HAL_StatusTypeDef CubeVCP_EnableRx(CubeVCP_Rx_Callback callback)
{
	static bool enabled = false;
	if (enabled) {
		LOGOMATIC_ERROR("CubeVCP_EnableRx: already enabled");
		return HAL_BUSY;
	} else {
		enabled = true;
	}

	if (vcp_uart_handle == NULL) {
		LOGOMATIC_ERROR("CubeVCP_EnableRx: vcp_uart_handle is NULL");
		return HAL_ERROR;
	}

	if (callback == NULL) {
		LOGOMATIC_ERROR("CubeVCP_EnableRx: callback is NULL");
		return HAL_ERROR;
	}

	vcp_rx_callback = callback;

	atomic_store_explicit(&vcp_rx_index, 0, memory_order_relaxed);

	HAL_StatusTypeDef status = HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1);

	if (status != HAL_OK) {
		LOGOMATIC_ERROR("CubeVCP_EnableRx: HAL_UART_Receive_IT failed");
		return status;
	}

	return HAL_OK;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance != vcp_uart_handle->Instance) {
		if (vcp_uart_rx_cplt_callback_other != NULL) {
			vcp_uart_rx_cplt_callback_other(huart);
		}
		return;
	}

	uint8_t received_byte = vcp_rx_byte;
	uint16_t index = vcp_rx_index;

	vcp_rx_buffer[index++] = received_byte;

	if (received_byte == '\n' || index >= CUBE_VCP_RX_BUFFER_SIZE) {
		uint16_t filled_len = index;

		CRITICAL_SECTION
		{
			for (uint16_t i = 0; i < filled_len; i++) {
				vcp_callback_staging[i] = vcp_rx_buffer[i];
			}
			vcp_rx_index = 0;
		}

		__HAL_UART_CLEAR_OREFLAG(huart);
		__HAL_UART_CLEAR_NEFLAG(huart);
		__HAL_UART_CLEAR_FEFLAG(huart);

		if (HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1) != HAL_OK) {
			huart->RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1);
		}

		if (vcp_rx_callback != NULL) {
			vcp_rx_callback(vcp_callback_staging, filled_len);
		}

	} else {
		CRITICAL_SECTION
		{
			vcp_rx_index = index;
		}

		__HAL_UART_CLEAR_OREFLAG(huart);
		if (HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1) != HAL_OK) {
			huart->RxState = HAL_UART_STATE_READY;
			HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1);
		}
	}
}
