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

	uint16_t index = atomic_load_explicit(&vcp_rx_index, memory_order_relaxed);
	if (index >= CUBE_VCP_RX_BUFFER_SIZE) {
		index = 0;
	}

	vcp_rx_buffer[index++] = vcp_rx_byte;

	atomic_store_explicit(&vcp_rx_index, index, memory_order_release);

	if (vcp_rx_byte == '\n' || index >= CUBE_VCP_RX_BUFFER_SIZE) {
		CRITICAL_SECTION
		{
			if (vcp_rx_callback != NULL) {
				vcp_rx_callback(vcp_rx_buffer, index);
			}
			atomic_store_explicit(&vcp_rx_index, 0, memory_order_relaxed);
		}
	}

	HAL_UART_Receive_IT(vcp_uart_handle, &vcp_rx_byte, 1);
}
