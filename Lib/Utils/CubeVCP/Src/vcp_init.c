#include "CubeVCP.h"
#include "CubeVCP_Config.h"
#include "Logomatic.h"
#include "PrivateInc/internal.h"
#include "main.h"
#include "usart.h"

HAL_StatusTypeDef CubeVCP_Setup(UART_HandleTypeDef *huart)
{
	static bool initialized = false;
	if (initialized) {
		LOGOMATIC_ERROR("CubeVCP_Setup: already initialized");
		return HAL_BUSY;
	} else {
		initialized = true;
	}

	if (huart == NULL) {
		LOGOMATIC_ERROR("CubeVCP_Setup: huart is NULL");
		return HAL_ERROR;
	}

	vcp_uart_handle = huart;

	atomic_store_explicit(&vcp_ring_head, 0, memory_order_relaxed);
	atomic_store_explicit(&vcp_ring_tail, 0, memory_order_relaxed);
	atomic_store_explicit(&vcp_tx_active, false, memory_order_relaxed);

	return HAL_OK;
}
