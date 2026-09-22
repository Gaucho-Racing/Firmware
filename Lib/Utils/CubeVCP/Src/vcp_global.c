#include "CubeVCP.h"
#include "PrivateInc/internal.h"

UART_HandleTypeDef *vcp_uart_handle = NULL;

__weak CubeVCP_UART_TxCpltCallback const vcp_uart_tx_cplt_callback_other = NULL;
__weak CubeVCP_UART_RxCpltCallback const vcp_uart_rx_cplt_callback_other = NULL;

CubeVCP_Rx_Callback vcp_rx_callback = NULL;

uint8_t vcp_tx_buffer[CUBE_VCP_TX_BUFFER_SIZE];

_Atomic uint16_t vcp_ring_head;
_Atomic uint16_t vcp_ring_tail;
_Atomic bool vcp_tx_active;

uint16_t vcp_rx_index;
uint8_t vcp_rx_byte;
uint8_t vcp_rx_buffer[CUBE_VCP_RX_BUFFER_SIZE];
uint8_t vcp_callback_staging[CUBE_VCP_RX_BUFFER_SIZE];
