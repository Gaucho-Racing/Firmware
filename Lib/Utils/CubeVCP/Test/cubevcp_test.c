#include <assert.h>
#include <string.h>

#include "CubeVCP.h"
#include "CubeVCP_Config.h"
#include "PrivateInc/internal.h"
#include "cubevcp_test_hal.h"

static uint8_t received_data[CUBE_VCP_RX_BUFFER_SIZE];
static uint16_t received_size;

static void receive_callback(const uint8_t *const data, const uint16_t size)
{
	received_size = size;
	memcpy(received_data, data, size);
}

int main(void)
{
	UART_HandleTypeDef uart = {.Instance = 1U, .RxState = HAL_UART_STATE_READY};
	CubeVCP_TestHal_Reset();

	assert(CubeVCP_Setup(&uart) == HAL_OK);
	assert(CubeVCP_Setup(&uart) == HAL_BUSY);
	assert(CubeVCP_SendString(NULL, 1U) == HAL_ERROR);

	assert(CubeVCP_SendString("abc", 3U) == HAL_OK);
	assert(cubevcp_test_hal.transmit_count == 1U);
	assert(cubevcp_test_hal.transmit_size == 3U);
	assert(memcmp(cubevcp_test_hal.transmit_data, "abc", 3U) == 0);

	assert(CubeVCP_SendString("def", 3U) == HAL_OK);
	assert(cubevcp_test_hal.transmit_count == 1U);
	uart.TxXferSize = 3U;
	HAL_UART_TxCpltCallback(&uart);
	assert(cubevcp_test_hal.transmit_count == 2U);
	assert(cubevcp_test_hal.transmit_size == 3U);
	assert(memcmp(cubevcp_test_hal.transmit_data, "def", 3U) == 0);

	uart.TxXferSize = 3U;
	HAL_UART_TxCpltCallback(&uart);
	assert(!atomic_load(&vcp_tx_active));

	assert(CubeVCP_EnableRx(receive_callback) == HAL_OK);
	assert(cubevcp_test_hal.receive_count == 1U);
	assert(cubevcp_test_hal.receive_size == 1U);
	vcp_rx_byte = 'o';
	HAL_UART_RxCpltCallback(&uart);
	vcp_rx_byte = 'k';
	HAL_UART_RxCpltCallback(&uart);
	vcp_rx_byte = '\n';
	HAL_UART_RxCpltCallback(&uart);
	assert(received_size == 3U);
	assert(memcmp(received_data, "ok\n", 3U) == 0);
	assert(cubevcp_test_hal.receive_count == 4U);
	assert(CubeVCP_EnableRx(receive_callback) == HAL_BUSY);

	return 0;
}
