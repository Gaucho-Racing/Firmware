#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN_Config.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_CUSTOM_ID.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "main.h"

#ifndef CUBEMX_CAN_H
#define CUBEMX_CAN_H

/**
 * @brief CAN handle for CubeCAN CAN.
 * @warning Do not access directly, use the provided API functions.
 */
typedef struct CubeCAN_Private_Handle CubeCAN_Handle;

/**
 * @brief CAN Identifier structure
 *
 * This structure is used to represent a CAN message identifier, which consists of a transmitting node ID, a receiving node ID, and a message ID. The structure is used in conjunction with the
 * Construct_Message_ID and Deconstruct_Message_ID functions to convert between the structure representation and the 32-bit integer representation of the CAN message identifier.
 *
 * @warning The structure does not represent custom IDs.
 * @warning The structure does not represent actual bit depth
 */
typedef struct {
	GRCAN_NODE_ID tx_node_id;
	GRCAN_NODE_ID rx_node_id;
	GRCAN_MSG_ID msg_id;
} CAN_Identifier;

/**
 * @brief Union for user-defined context data in CubeCAN configuration.
 *
 * This union allows for flexible user-defined context data to be passed to the receive callback function. It can hold either a pointer to any user-defined data or a specific GRCAN_BUS_ID value.
 *
 * @warning The union should be used carefully to ensure that the correct type of data is accessed in the callback function.
 *
 * @note Compile time checks verify that GRCAN_BUS_ID is the same size or smaller than the pointer, ensuring safe usage of the union.
 */
typedef union {
	/// @brief Pointer to user-defined context data. This can be used to pass additional data or state information to the receive callback function.
	void *full_user_context;
	/// @brief GRCAN_BUS_ID value for user-defined context data. This can be used to pass the bus ID to the receive callback function.
	GRCAN_BUS_ID busid_user_context;
} CubeCAN_Config_Context;

/**
 * @brief Callback function type for receiving CAN messages.
 *
 * @warning The callback function should not perform blocking operations or take too long to execute.
 * @warning The callback function should not call any CubeCAN functions, as it may lead to undefined behavior.
 * @warning It is the responsibility of the callback function to verify the integrity of the received data and handle any errors or null inputs.
 */
typedef void (*CubeCAN_RxCallback)(const CubeCAN_Config_Context *const context, const CAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size);

/**
 * @brief Configuration structure for CubeCAN CAN.
 *
 * This structure is used to configure the CubeCAN CAN handle, including the receive callback function and user context. The receive callback function is called when a CAN message is received, and the
 * user context is a pointer to user-defined data that can be passed to the callback function.
 *
 * @warning Do not edit after initialization, as it may lead to undefined behavior.
 */
typedef struct {
	/// @brief Union for user-defined context data. This can be used to pass additional data or state information to the receive callback function.
	CubeCAN_Config_Context context;
	/// @brief Callback function for receiving CAN messages. This function is called when a CAN message is received.
	CubeCAN_RxCallback rx_callback;
	/// @brief Node ID of the sending device. This is used to identify the source of the CAN messages and must be unique on the CAN bus.
	GRCAN_NODE_ID sending_node_id;
} CubeCAN_Config;

/**
 * @brief Initializes a CubeCAN CAN handle with the given FDCAN handle and configuration, and starts the CAN peripheral.
 * @param hfdcan Pointer to the FDCAN handle.
 * @param config Pointer to the CubeCAN configuration structure.
 * @return Pointer to the initialized CubeCAN CAN handle, or NULL if initialization fails.
 * @note This functions wraps <c>CubeCAN_Private_Init</c> and <c>CubeCAN_Private_Start</c> into a single call. If either of those functions fails, this function will return NULL and the handle will be
 * released.
 */
CubeCAN_Handle *CubeCAN_Entrance(FDCAN_HandleTypeDef *hfdcan, CubeCAN_Config *config);

/**
 * @brief Stops the CubeCAN CAN peripheral and releases the associated handle.
 * @param handle Pointer to the CubeCAN CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 * @note This function can be used regardless of if <c>CubeCAN_Entrance</c> was used to initialize the handle.
 */
HAL_StatusTypeDef CubeCAN_Exit(CubeCAN_Handle *handle);

/**
 * @brief Processes periodic tasks for the CubeCAN CAN handle, such as handling timeouts and managing the transmission queue.
 *
 * For each configured bus, this function will send one CAN message per call. It is recommended to call this function at a regular interval to ensure timely processing of CAN messages.
 *
 * @note This function sends one can message per configured bus per call. Not calling it will simply not send any messages.
 * @warning It is recommended to call this function no more than and no less than once every 625 us.
 * @warning This function checks during early boot that it is not called more than ABSOLUTE_MAX_INVOCATIONS_PER_TICK times per millisecond, failing if so
 */
void CubeCAN_Tick(void);

/**
 * @brief Sends a CAN message using the CubeCAN CAN handle, with the specified receive node, message ID, data payload, and size.
 * @param handle Pointer to the CubeCAN CAN handle.
 * @param rx_node The receive node identifier for the message.
 * @param msg_id The message ID for the message.
 * @param data Pointer to the data payload of the message.
 * @param size The size of the data payload in bytes.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 * @note The size of the data payload must not exceed FDCAN_MAX_DATA_BYTES (64 bytes). If the size exceeds this limit, the function will return HAL_ERROR.
 */
HAL_StatusTypeDef CubeCAN_Send(CubeCAN_Handle *const handle, const GRCAN_NODE_ID rx_node, const GRCAN_MSG_ID msg_id, const void *const data, const uint8_t size);

/**
 * @brief Constructs a CAN message identifier from the given transmitting node ID, receiving node ID, and message ID.
 *
 * @param identifier Pointer to the CAN_Identifier structure containing the node and message IDs.
 *
 * @return The constructed 29-bit CAN message extended identifier.
 */
uint32_t CubeCAN_Construct_Identifier(const CAN_Identifier *const identifier);

/**
 * @brief Deconstructs a 29-bit CAN message extended identifier into its constituent transmitting node ID, receiving node ID, and message ID.
 *
 * @param message_id The 29-bit CAN message extended identifier to be deconstructed.
 *
 * @return A CAN_Identifier structure containing the deconstructed node and message IDs.
 *
 * @warning The function does not guarantee that the returned structure will represent a valid CAN message identifier.
 * @warning The function does not support custom IDs.
 */
CAN_Identifier CubeCAN_Deconstruct_Identifier(const uint32_t message_id);

#endif
