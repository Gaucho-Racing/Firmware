#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#include "CubeVCP.h"
#include "PrivateInc/internal.h"
#include "main.h"
#include "usart.h"

HAL_StatusTypeDef CubeVCP_SendString(const char *const str, uint16_t length)
{
	if (vcp_uart_handle == NULL || str == NULL || length == 0) {
		return HAL_ERROR;
	}

	uint16_t current_head = atomic_load_explicit(&vcp_ring_head, memory_order_relaxed);
	uint16_t current_tail = atomic_load_explicit(&vcp_ring_tail, memory_order_acquire);
	uint16_t used_space = current_head - current_tail;
	uint16_t free_space = CUBE_VCP_TX_BUFFER_SIZE - used_space;

	if (length > free_space) {
		length = free_space;
	}

	if (length == 0) {
		return HAL_BUSY;
	}

	uint16_t local_head = current_head;
	for (uint16_t i = 0; i < length; i++) {
		vcp_tx_buffer[local_head & VCP_TX_BUFFER_MASK] = (uint8_t)str[i];
		local_head++;
	}

	atomic_store_explicit(&vcp_ring_head, local_head, memory_order_release);

	if (!atomic_load_explicit(&vcp_tx_active, memory_order_relaxed)) {
		if (!atomic_exchange_explicit(&vcp_tx_active, true, memory_order_acquire)) {
			/* We successfully set tx_active to true */
			uint16_t current_tail = atomic_load_explicit(&vcp_ring_tail, memory_order_acquire);
			uint16_t current_head = atomic_load_explicit(&vcp_ring_head, memory_order_acquire);

			if (current_tail != current_head) {
				uint16_t tail_idx = current_tail & VCP_TX_BUFFER_MASK;
				uint16_t head_idx = current_head & VCP_TX_BUFFER_MASK;
				uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_TX_BUFFER_SIZE - tail_idx);

				HAL_StatusTypeDef status = HAL_UART_Transmit_IT(vcp_uart_handle, &vcp_tx_buffer[tail_idx], send_size);
				if (status != HAL_OK) {
					atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
					return status;
				}
			} else {
				/* No data to send, clear the active flag */
				atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
			}
		}
	}

	return HAL_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (vcp_uart_handle == NULL || huart->Instance != vcp_uart_handle->Instance) {
		if (vcp_uart_tx_cplt_callback_other != NULL) {
			vcp_uart_tx_cplt_callback_other(huart);
		}
		return;
	}

	uint16_t current_tail = atomic_load_explicit(&vcp_ring_tail, memory_order_relaxed) + huart->TxXferSize;
	atomic_store_explicit(&vcp_ring_tail, current_tail, memory_order_release);

	uint16_t current_head = atomic_load_explicit(&vcp_ring_head, memory_order_acquire);

	if (current_tail != current_head) {
		uint16_t tail_idx = current_tail & VCP_TX_BUFFER_MASK;
		uint16_t head_idx = current_head & VCP_TX_BUFFER_MASK;

		uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_TX_BUFFER_SIZE - tail_idx);

		if (HAL_UART_Transmit_IT(vcp_uart_handle, &vcp_tx_buffer[tail_idx], send_size) != HAL_OK) {
			atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
		}
	} else {
		atomic_store_explicit(&vcp_tx_active, false, memory_order_release);

		current_head = atomic_load_explicit(&vcp_ring_head, memory_order_acquire);
		if (current_tail != current_head) {
			if (!atomic_load_explicit(&vcp_tx_active, memory_order_relaxed)) {
				atomic_store_explicit(&vcp_tx_active, true, memory_order_relaxed);

				uint16_t tail_idx = current_tail & VCP_TX_BUFFER_MASK;
				uint16_t head_idx = current_head & VCP_TX_BUFFER_MASK;
				uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_TX_BUFFER_SIZE - tail_idx);

				if (HAL_UART_Transmit_IT(vcp_uart_handle, &vcp_tx_buffer[tail_idx], send_size) != HAL_OK) {
					atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
				}
			}
		}
	}
}
