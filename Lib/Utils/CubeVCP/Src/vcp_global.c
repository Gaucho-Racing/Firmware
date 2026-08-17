#include "CubeVCP.h"
#include "PrivateInc/internal.h"

__weak UART_HandleTypeDef *const vcp_uart_handle = NULL;

__weak CubeVCP_UART_TxCpltCallback const vcp_uart_tx_cplt_callback_other = NULL;

CubeVCP_Rx_Callback vcp_rx_callback = NULL;

uint8_t vcp_tx_buffer[CUBE_VCP_TX_BUFFER_SIZE];

_Atomic uint16_t vcp_ring_head = 0;
_Atomic uint16_t vcp_ring_tail = 0;
_Atomic bool vcp_tx_active = false;

_Atomic uint16_t vcp_rx_index = 0;
uint8_t vcp_rx_byte;
uint8_t vcp_rx_buffer[CUBE_VCP_RX_BUFFER_SIZE];
