#include "cubecan_test_hal.h"

#include <pthread.h>
#include <string.h>

#include "Unused.h"
#include "main.h"

CubeCAN_TestHal cubecan_test_hal;

pthread_mutex_t __mock_global_irq_mutex;
_Thread_local uint32_t __mock_primask_state = 0U;
_Thread_local uint32_t __mock_irq_nesting_depth = 0U;

__attribute__((constructor)) static void initialize_test_mutex(void)
{
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init(&attributes);
	pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&__mock_global_irq_mutex, &attributes);
	pthread_mutexattr_destroy(&attributes);
}

static uint32_t tick;

void CubeCAN_TestHal_Reset(void)
{
	memset(&cubecan_test_hal, 0, sizeof(cubecan_test_hal));
	cubecan_test_hal.tx_fifo_free_level = 32U;
	cubecan_test_hal.activate_status = HAL_OK;
	cubecan_test_hal.deactivate_status = HAL_OK;
	cubecan_test_hal.start_status = HAL_OK;
	cubecan_test_hal.stop_status = HAL_OK;
	cubecan_test_hal.init_status = HAL_OK;
	cubecan_test_hal.tx_status = HAL_OK;
	cubecan_test_hal.rx_status = HAL_OK;
	cubecan_test_hal.protocol_status_result = HAL_OK;
	cubecan_test_hal.restricted_exit_status = HAL_OK;
	tick = 0U;
}

void CubeCAN_TestHal_QueueRx(const FDCAN_RxHeaderTypeDef *header, const uint8_t *data, uint8_t size)
{
	const uint32_t index = cubecan_test_hal.rx_count_pending++;
	cubecan_test_hal.rx_headers[index] = *header;
	cubecan_test_hal.rx_sizes[index] = size;
	memcpy(cubecan_test_hal.rx_data[index], data, size);
}

void CubeCAN_TestHal_AdvanceTick(void)
{
	++tick;
}

uint32_t HAL_GetTick(void)
{
	return tick;
}
void Error_Handler(void)
{
	cubecan_test_hal.error_handler_count++;
}

HAL_StatusTypeDef HAL_FDCAN_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t interrupts, uint32_t buffer)
{
	UNUSED(hfdcan);
	cubecan_test_hal.activate_count++;
	cubecan_test_hal.last_interrupts = interrupts;
	cubecan_test_hal.last_tx_buffer = buffer;
	return cubecan_test_hal.activate_status;
}

HAL_StatusTypeDef HAL_FDCAN_DeactivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t interrupts)
{
	UNUSED(hfdcan);
	cubecan_test_hal.deactivate_count++;
	cubecan_test_hal.last_interrupts = interrupts;
	return cubecan_test_hal.deactivate_status;
}

HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan)
{
	cubecan_test_hal.start_count++;
	if (cubecan_test_hal.start_status == HAL_OK) {
		hfdcan->State = HAL_FDCAN_STATE_BUSY;
	}
	return cubecan_test_hal.start_status;
}

HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan)
{
	cubecan_test_hal.stop_count++;
	if (cubecan_test_hal.stop_status == HAL_OK) {
		hfdcan->State = HAL_FDCAN_STATE_RESET;
	}
	return cubecan_test_hal.stop_status;
}

HAL_StatusTypeDef HAL_FDCAN_Init(FDCAN_HandleTypeDef *hfdcan)
{
	UNUSED(hfdcan);
	cubecan_test_hal.init_count++;
	return cubecan_test_hal.init_status;
}

HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *header, const uint8_t *data)
{
	const uint32_t index = cubecan_test_hal.tx_count++;
	cubecan_test_hal.tx_handles[index] = hfdcan;
	cubecan_test_hal.tx_headers[index] = *header;
	uint8_t data_size = 0U;
	if (header->DataLength <= FDCAN_DLC_BYTES_8) {
		data_size = (uint8_t)header->DataLength;
	} else {
		switch (header->DataLength) {
			case FDCAN_DLC_BYTES_12:
				data_size = 12U;
				break;
			case FDCAN_DLC_BYTES_16:
				data_size = 16U;
				break;
			case FDCAN_DLC_BYTES_20:
				data_size = 20U;
				break;
			case FDCAN_DLC_BYTES_24:
				data_size = 24U;
				break;
			case FDCAN_DLC_BYTES_32:
				data_size = 32U;
				break;
			case FDCAN_DLC_BYTES_48:
				data_size = 48U;
				break;
			case FDCAN_DLC_BYTES_64:
				data_size = 64U;
				break;
			default:
				break;
		}
	}
	memcpy(cubecan_test_hal.tx_data[index], data, data_size);
	return cubecan_test_hal.tx_status;
}

uint32_t HAL_FDCAN_GetTxFifoFreeLevel(FDCAN_HandleTypeDef *hfdcan)
{
	UNUSED(hfdcan);
	return cubecan_test_hal.tx_fifo_free_level;
}

uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *hfdcan, uint32_t fifo)
{
	UNUSED(hfdcan);
	UNUSED(fifo);
	return cubecan_test_hal.rx_count_pending - cubecan_test_hal.rx_count;
}

HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t fifo, FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
	UNUSED(hfdcan);
	UNUSED(fifo);
	const uint32_t index = cubecan_test_hal.rx_count++;
	*header = cubecan_test_hal.rx_headers[index];
	memcpy(data, cubecan_test_hal.rx_data[index], cubecan_test_hal.rx_sizes[index]);
	return cubecan_test_hal.rx_status;
}

HAL_FDCAN_StateTypeDef HAL_FDCAN_GetState(FDCAN_HandleTypeDef *hfdcan)
{
	return hfdcan->State;
}

HAL_StatusTypeDef HAL_FDCAN_GetProtocolStatus(FDCAN_HandleTypeDef *hfdcan, FDCAN_ProtocolStatusTypeDef *status)
{
	*status = hfdcan->ProtocolStatus;
	return cubecan_test_hal.protocol_status_result;
}

uint32_t HAL_FDCAN_IsRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan)
{
	UNUSED(hfdcan);
	return cubecan_test_hal.restricted_operation;
}

HAL_StatusTypeDef HAL_FDCAN_ExitRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan)
{
	UNUSED(hfdcan);
	cubecan_test_hal.restricted_exit_count++;
	return cubecan_test_hal.restricted_exit_status;
}
