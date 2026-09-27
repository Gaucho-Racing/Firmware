#include "cubevcp_test_hal.h"

#include <pthread.h>
#include <string.h>

CubeVCP_TestHal cubevcp_test_hal;

pthread_mutex_t __mock_global_irq_mutex;
_Thread_local uint32_t __mock_primask_state;
_Thread_local uint32_t __mock_irq_nesting_depth;

__attribute__((constructor)) static void initialize_test_mutex(void)
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&__mock_global_irq_mutex, &attributes);
	pthread_mutexattr_destroy(&attributes);
}

void CubeVCP_TestHal_Reset(void)
{
	memset(&cubevcp_test_hal, 0, sizeof(cubevcp_test_hal));
	cubevcp_test_hal.transmit_status = HAL_OK;
	cubevcp_test_hal.receive_status = HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size)
{
	const uint16_t copy_size = size < sizeof(cubevcp_test_hal.transmit_data) ? size : sizeof(cubevcp_test_hal.transmit_data);
	huart->TxXferSize = size;
	cubevcp_test_hal.transmit_count++;
	cubevcp_test_hal.transmit_size = size;
	memcpy(cubevcp_test_hal.transmit_data, data, copy_size);
	return cubevcp_test_hal.transmit_status;
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *data, uint16_t size)
{
	(void)huart;
	cubevcp_test_hal.receive_count++;
	cubevcp_test_hal.receive_buffer = data;
	cubevcp_test_hal.receive_size = size;
	return cubevcp_test_hal.receive_status;
}
