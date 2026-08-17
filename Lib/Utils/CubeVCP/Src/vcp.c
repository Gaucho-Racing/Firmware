#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#include "CubeVCP.h"
#include "main.h"
#include "usart.h"

#define CUBE_VCP_BUFFER_SIZE 256
#define BUFFER_MASK (CUBE_VCP_BUFFER_SIZE - 1)

static uint8_t ring_buffer[CUBE_VCP_BUFFER_SIZE];

static _Atomic uint16_t head = 0;
static _Atomic uint16_t tail = 0;
static _Atomic bool vcp_tx_active = false;

__weak UART_HandleTypeDef *vcp_uart_handle = &hlpuart1;

void VCP_SendString(const char *const str, uint16_t length)
{
	if (vcp_uart_handle == NULL || str == NULL || length == 0) {
		return;
	}

	uint16_t current_head = atomic_load_explicit(&head, memory_order_relaxed);
	uint16_t current_tail = atomic_load_explicit(&tail, memory_order_acquire);
	uint16_t used_space = current_head - current_tail;
	uint16_t free_space = CUBE_VCP_BUFFER_SIZE - used_space;

	if (length > free_space) {
		length = free_space;
	}

	if (length == 0) {
		return;
	}

	uint16_t local_head = current_head;
	for (uint16_t i = 0; i < length; i++) {
		ring_buffer[local_head & BUFFER_MASK] = (uint8_t)str[i];
		local_head++;
	}

	atomic_store_explicit(&head, local_head, memory_order_release);

	if (!atomic_load_explicit(&vcp_tx_active, memory_order_relaxed)) {
		atomic_thread_fence(memory_order_acquire);
		atomic_store_explicit(&vcp_tx_active, true, memory_order_relaxed);

		current_tail = atomic_load_explicit(&tail, memory_order_relaxed);

		uint16_t tail_idx = current_tail & BUFFER_MASK;
		uint16_t head_idx = local_head & BUFFER_MASK;

		uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_BUFFER_SIZE - tail_idx);

		if (HAL_UART_Transmit_IT(vcp_uart_handle, &ring_buffer[tail_idx], send_size) != HAL_OK) {
			atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
		}
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (vcp_uart_handle == NULL || huart->Instance != vcp_uart_handle->Instance) {
		return;
	}

	uint16_t current_tail = atomic_load_explicit(&tail, memory_order_relaxed) + huart->TxXferSize;
	atomic_store_explicit(&tail, current_tail, memory_order_release);

	uint16_t current_head = atomic_load_explicit(&head, memory_order_acquire);

	if (current_tail != current_head) {
		uint16_t tail_idx = current_tail & BUFFER_MASK;
		uint16_t head_idx = current_head & BUFFER_MASK;

		uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_BUFFER_SIZE - tail_idx);

		if (HAL_UART_Transmit_IT(vcp_uart_handle, &ring_buffer[tail_idx], send_size) != HAL_OK) {
			atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
		}
	} else {
		atomic_store_explicit(&vcp_tx_active, false, memory_order_release);

		current_head = atomic_load_explicit(&head, memory_order_acquire);
		if (current_tail != current_head) {
			if (!atomic_load_explicit(&vcp_tx_active, memory_order_relaxed)) {
				atomic_store_explicit(&vcp_tx_active, true, memory_order_relaxed);

				uint16_t tail_idx = current_tail & BUFFER_MASK;
				uint16_t head_idx = current_head & BUFFER_MASK;
				uint16_t send_size = (head_idx > tail_idx) ? (head_idx - tail_idx) : (CUBE_VCP_BUFFER_SIZE - tail_idx);

				if (HAL_UART_Transmit_IT(vcp_uart_handle, &ring_buffer[tail_idx], send_size) != HAL_OK) {
					atomic_store_explicit(&vcp_tx_active, false, memory_order_release);
				}
			}
		}
	}
}
