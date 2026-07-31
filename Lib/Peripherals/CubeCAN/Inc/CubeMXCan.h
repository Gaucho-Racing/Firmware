#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN_Config.h"
#include "CubeMXCanExt.h"
#include "main.h"

#ifndef CUBEMX_CAN_H
#define CUBEMX_CAN_H

#define FDCAN_MAX_DATA_BYTES 64U

/// @brief CAN handle for CubeMX CAN. @warning Do not access directly, use the provided API functions.
typedef struct CubeMXCan_Private_Handle CubeMXCan_Handle;

/// @brief Callback function type for receiving CAN messages. @warning Do not call directly, use the provided API functions.
typedef void (*CubeCAN_RxCallback)(const void *const user_context, const CAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size);

/**
 * @brief Configuration structure for CubeMX CAN.
 *
 * This structure is used to configure the CubeMX CAN handle, including the receive callback function and user context. The receive callback function is called when a CAN message is received, and the
 * user context is a pointer to user-defined data that can be passed to the callback function.
 *
 * @warning Do not edit after initialization, as it may lead to undefined behavior.
 */
typedef struct {
	/// @brief User-defined context pointer that can be passed to the receive callback function. This can be used to pass additional data or state information to the callback function.
	void *user_context;
	/// @brief Callback function for receiving CAN messages. This function is called when a CAN message is received.
	CubeCAN_RxCallback rx_callback;
	/// @brief Node ID of the sending device. This is used to identify the source of the CAN messages and must be unique on the CAN bus.
	GRCAN_NODE_ID sending_node_id;
} CubeCAN_Config;

/**
 * @brief Initializes a CubeMX CAN handle with the given FDCAN handle and configuration, and starts the CAN peripheral.
 * @param hfdcan Pointer to the FDCAN handle.
 * @param config Pointer to the CubeCAN configuration structure.
 * @return Pointer to the initialized CubeMX CAN handle, or NULL if initialization fails.
 * @note This functions wraps <c>CubeMXCan_Init</c> and <c>CubeMXCan_Start</c> into a single call. If either of those functions fails, this function will return NULL and the handle will be released.
 */
CubeMXCan_Handle *CubeMXCan_OneShotInitStart(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config);

/**
 * @brief Stops the CubeMX CAN peripheral and releases the associated handle.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 * @note This functions wraps <c>CubeMXCan_Stop</c> and <c>CubeMXCan_Release</c> into a single call. If either of those functions fails, this function will return the error code and the handle will
 * not be released.
 * @note This function can be used regardless of if <c>CubeMXCan_OneShotInitStart</c> was used to initialize the handle.
 */
HAL_StatusTypeDef CubeMXCan_OneShotReleaseStop(CubeMXCan_Handle *handle);

/**
 * @brief Initializes a CubeMX CAN handle with the given FDCAN handle and configuration.
 * @param hfdcan Pointer to the FDCAN handle.
 * @param config Pointer to the CubeCAN configuration structure.
 * @return Pointer to the initialized CubeMX CAN handle, or NULL if initialization fails.
 */
CubeMXCan_Handle *CubeMXCan_Init(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config);

/**
 * @brief Starts the CubeMX CAN handle, enabling message transmission and reception.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Start(CubeMXCan_Handle *const handle);

/**
 * @brief Stops the CubeMX CAN handle, disabling message transmission and reception.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Stop(CubeMXCan_Handle *const handle);

/**
 * @brief Releases the resources associated with the CubeMX CAN handle.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Release(CubeMXCan_Handle *const handle);

/**
 * @brief Adds a filter to the CubeMX CAN handle, allowing for selective message reception based on the specified filter criteria.
 * @param handle Pointer to the CubeMX CAN handle.
 * @param filter Pointer to the FDCAN filter configuration structure.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_AddFilter(const CubeMXCan_Handle *const handle, const FDCAN_FilterTypeDef *const filter);

/**
 * @brief Processes periodic tasks for the CubeMX CAN handle, such as handling timeouts and managing the transmission queue.
 * @warning This function sends one can message per configured bus per call. Not calling it will simply not send any messages.
 */
void CubeMXCan_Tick(void);

/**
 * @brief Sends a CAN message using the CubeMX CAN handle, with the specified receive node, message ID, data payload, and size.
 * @param handle Pointer to the CubeMX CAN handle.
 * @param rx_node The receive node identifier for the message.
 * @param msg_id The message ID for the message.
 * @param data Pointer to the data payload of the message.
 * @param size The size of the data payload in bytes.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 * @note The size of the data payload must not exceed FDCAN_MAX_DATA_BYTES (64 bytes). If the size exceeds this limit, the function will return HAL_ERROR.
 */
HAL_StatusTypeDef CubeMXCan_Send(CubeMXCan_Handle *const handle, const GRCAN_NODE_ID rx_node, const GRCAN_MSG_ID msg_id, const void *const data, const uint8_t size);

#endif
