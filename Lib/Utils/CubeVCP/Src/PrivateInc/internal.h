#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeVCP_Config.h"

#ifndef CUBE_VCP_INTERNAL_H
#define CUBE_VCP_INTERNAL_H

/**
 * @brief Callback function pointer for handling received data in CubeVCP.
 *
 * This function pointer allows users to define their own callback function to handle received data in CubeVCP.
 * The callback function will be invoked either off of buffers or off of individual bytes, depending on the configuration of CubeVCP.
 *
 * @warning The callback function should be thread-safe and should not block for long periods of time
 */
extern CubeVCP_Rx_Callback vcp_rx_callback;

/**
 * @brief Pointer to the UART handle used for VCP transmission.
 *
 * This pointer should be set to the UART handle that is used for VCP transmission.
 * It is used internally by CubeVCP to manage the transmission of data over the VCP interface.
 * This can be LPUART, USART, or any other UART peripheral that is configured for VCP transmission.
 *
 * @note This variable should not be modified once setup by the user.
 * @warning The UART handle should be properly initialized and configured before using CubeVCP functions by STM32CubeMX.
 */
extern UART_HandleTypeDef *vcp_uart_handle;

/**
 * @brief Mask for the ring buffer used in VCP transmission.
 *
 * This mask is used to wrap the head and tail indices of the ring buffer to ensure they stay within the bounds of the buffer size.
 *
 * @note The size of the ring buffer must be a power of two for this mask to work correctly
 */
#define VCP_TX_BUFFER_MASK (CUBE_VCP_TX_BUFFER_SIZE - 1)

/**
 * @brief Head of the ring buffer for VCP transmission.
 *
 * This atomic variable keeps track of the current head index in the ring buffer
 * It is used to determine where new data should be written in the buffer
 *
 * @warning Requires atomic operations to ensure thread safety when accessed from multiple contexts
 */
extern _Atomic uint16_t vcp_ring_head;

/**
 * @brief Tail of the ring buffer for VCP transmission.
 *
 * This atomic variable keeps track of the current tail index in the ring buffer
 * It is used to determine where data should be read from the buffer for transmission
 *
 * @warning Requires atomic operations to ensure thread safety when accessed from multiple contexts
 */
extern _Atomic uint16_t vcp_ring_tail;

/**
 * @brief Flag indicating whether a VCP transmission is currently active
 *
 * This atomic boolean variable is set to true when a transmission is in progress and false when the transmission is complete
 *
 * @warning Requires atomic operations to ensure thread safety when accessed from multiple contexts
 */
extern _Atomic bool vcp_tx_active;

/**
 * @brief Ring buffer for VCP transmission.
 *
 * This buffer is used to store data that is to be transmitted over the VCP interface
 *
 * @note Access only through atomic operations to ensure thread safety when accessed from multiple contexts
 * @warning The size of this buffer is defined by CUBE_VCP_TX_BUFFER_SIZE and must be a power of two for proper operation
 */
extern uint8_t vcp_tx_buffer[];

/**
 * @brief The next byte to be received in the VCP reception buffer.
 *
 * This variable is used to store the byte that has just been received from the UART interface before it is placed into the reception buffer.
 *
 * @warning Only the UART interrupt handler should modify this variable to ensure correct operation.
 */
extern uint8_t vcp_rx_byte;

/**
 * @brief Reception buffer for VCP.
 *
 * This buffer is used to store incoming data received over the VCP interface.
 *
 * @note This is a simple buffer and does not implement a ring buffer mechanism. It is the responsibility of the user to manage the data in this buffer appropriately.
 * @remark Access only through atomic operations to ensure thread safety when accessed from multiple contexts
 * @warning The size of this buffer is defined by CUBE_VCP_RX_BUFFER_SIZE but it does not have to be since it is not a ring buffer
 */
extern uint8_t vcp_rx_buffer[];

/**
 * @brief Index of the next byte to be written in the VCP reception buffer.
 *
 * This atomic variable is used to keep track of the index where the next byte should be written in the VCP reception buffer.
 *
 * @warning Requires atomic operations to ensure thread safety when accessed from multiple contexts
 */
extern _Atomic uint16_t vcp_rx_index;

#endif
