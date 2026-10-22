#include <stdint.h>

#include "main.h"

#ifndef CUBE_VCP_H
#define CUBE_VCP_H

/**
 * @brief Callback function type for UART transmission complete events in CubeVCP.
 * @param huart Pointer to the UART handle.
 */
typedef void (*CubeVCP_UART_TxCpltCallback)(UART_HandleTypeDef *huart);

/**
 * @brief Callback function type for UART reception complete events in CubeVCP.
 * @param huart Pointer to the UART handle.
 */
typedef void (*CubeVCP_UART_RxCpltCallback)(UART_HandleTypeDef *huart);

/**
 * @brief Callback function type for handling received data in CubeVCP.
 * @param buffer Pointer to the received data buffer.
 * @param length Length of the received data in bytes.
 */
typedef void (*CubeVCP_Rx_Callback)(const uint8_t *const buffer, const uint16_t length);

/**
 * @brief Callback function pointer for handling UART transmission complete events in CubeVCP that are not related to the VCP transmission
 *
 * This function pointer allows users to define their own callback function to handle UART transmission complete events that are not related to the VCP transmission.
 * It is called when a UART transmission complete event occurs for a UART instance that is not the one used for VCP transmission.
 *
 * @note This variable should not be modified once setup by the user.
 * @warning The callback function should be thread-safe and should not block for long periods of time
 */
extern __weak CubeVCP_UART_TxCpltCallback const vcp_uart_tx_cplt_callback_other;

/**
 * @brief Callback function pointer for handling UART reception complete events in CubeVCP that are not related to the VCP transmission
 *
 * This function pointer allows users to define their own callback function to handle UART reception complete events that are not related to the VCP transmission.
 * It is called when a UART reception complete event occurs for a UART instance that is not the one used for VCP transmission.
 *
 * @note This variable should not be modified once setup by the user.
 * @warning The callback function should be thread-safe and should not block for long periods of time
 */
extern __weak CubeVCP_UART_RxCpltCallback const vcp_uart_rx_cplt_callback_other;

/**
 * @brief Initialize CubeVCP with the UART handle to be used for VCP communication.
 * @param huart Pointer to the UART handle that will be used for VCP transmission and reception.
 * @retval HAL_StatusTypeDef HAL_OK if initialization was successful, HAL_ERROR if huart is NULL, or HAL_BUSY if already initialized.
 * @note This function MUST be called first before any other CubeVCP functions.
 * @note The UART handle must be properly initialized and configured by STM32CubeMX before calling this function.
 */
HAL_StatusTypeDef CubeVCP_Setup(UART_HandleTypeDef *huart);

/**
 * @brief Enable reception of data over the VCP interface and set the callback function to handle received data.
 * @param callback Pointer to the callback function that will be called when data is received over the VCP interface.
 * @retval HAL_StatusTypeDef HAL_OK if the reception was successfully enabled, HAL_ERROR if there was an error, or HAL_BUSY if the reception is already enabled.
 * @note This function should be called after CubeVCP_Setup() and after the UART handle has been properly initialized and configured by STM32CubeMX.
 * @note RX uses double buffering, so data reception continues uninterrupted while the callback processes data.
 * @warning The callback function should be thread-safe and should not block for long periods of time, you easily can lose data on long messages if not properly configured.
 */
HAL_StatusTypeDef CubeVCP_EnableRx(CubeVCP_Rx_Callback callback);

/**
 * @brief Send a string over the VCP interface
 * @param data Pointer to the string data to be sent
 * @param length Length of the string data to be sent
 * @retval HAL_StatusTypeDef HAL_OK if the string was successfully queued for transmission, HAL_ERROR if there was an error, or HAL_BUSY if the internal buffer is full
 * @note This function is non-blocking and will return immediately. The data will be sent in the background using interrupts.
 * @note The data buffer does not have to remain valid after the function returns, as the data will be copied into an internal buffer for transmission.
 * @note The maximum length of the data that can be sent is defined by CUBE_VCP_TX_BUFFER_SIZE. If the length of the data exceeds this value, only the first CUBE_VCP_TX_BUFFER_SIZE bytes will be
 * sent.
 */
HAL_StatusTypeDef CubeVCP_SendString(const char *const data, uint16_t length);

#endif
