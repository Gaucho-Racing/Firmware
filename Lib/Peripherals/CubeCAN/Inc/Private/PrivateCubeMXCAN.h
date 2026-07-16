#include <stdbool.h>
#include <stdint.h>

#include "CubeMXCan.h"
#include "main.h"

#ifndef PRIVATE_CUBE_MX_CAN_H
#define PRIVATE_CUBE_MX_CAN_H

/**
 * @brief CubeMX CAN handle structure
 *
 * This structure is used to represent a CubeMX CAN handle, which consists of a pointer to the FDCAN handle, a configuration structure, a transmission queue, and other relevant parameters.\
 *
 * @warning Use the provided API functions to interact with the CubeMX CAN handle.
 * @warning The structure is intended for internal use only and should not be accessed directly by user code.
 */
typedef struct CubeMXCan_Handle {
	/// @brief Pointer to the FDCAN handle associated with this CubeMX CAN handle.
	FDCAN_HandleTypeDef *hfdcan;
	/// @brief Configuration structure for the CubeMX CAN handle, containing the receive callback and user context.
	CubeCAN_Config config;
	/// @brief Transmission queue for the CubeMX CAN handle, containing the messages to be transmitted.
	GRCAN_TxMessage tx_queue[CUBEMX_CAN_TX_QUEUE_SIZE];
	/// @brief Index of the head of the transmission queue, indicating the next message to be transmitted.
	uint32_t tx_head;
	/// @brief Count of messages in the transmission queue, indicating the number of messages waiting to be transmitted.
	uint32_t tx_count;
	/// @brief Flag indicating whether the CubeMX CAN handle has been started.
	bool started;
} CubeMXCan_Handle;

/// @brief Maximum number of CubeMX CAN instances supported by the library.
#define CUBEMX_CAN_MAX_INSTANCES 3U

/**
 * @brief Retrieves the CubeMX CAN handle associated with the given FDCAN handle.
 * @param hfdcan Pointer to the FDCAN handle.
 * @return Pointer to the CubeMX CAN handle associated with the given FDCAN handle
 */
CubeMXCan_Handle *CubeMXCan_Private_GetHandle(FDCAN_HandleTypeDef *hfdcan);

/**
 * @brief Registers a CubeMX CAN handle with the given FDCAN handle.
 * @param hfdcan Pointer to the FDCAN handle.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Private_RegisterHandle(FDCAN_HandleTypeDef *hfdcan, CubeMXCan_Handle *handle);

/**
 * @brief Unregisters the CubeMX CAN handle associated with the given FDCAN handle.
 * @param hfdcan Pointer to the FDCAN handle.
 */
void CubeMXCan_Private_UnregisterHandle(FDCAN_HandleTypeDef *hfdcan);

/**
 * @brief Sends a queued message from the CubeMX CAN handle.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(CubeMXCan_Handle *handle);

/**
 * @brief Dispatches a received message from the CubeMX CAN handle.
 * @param hfdcan Pointer to the FDCAN handle.
 */
void CubeMXCan_Private_DispatchRx(FDCAN_HandleTypeDef *hfdcan);

/**
 * @brief Converts a Data Length Code (DLC) to the corresponding number of bytes.
 * @param dlc The Data Length Code to be converted.
 * @return The number of bytes corresponding to the given DLC.
 */
uint8_t CubeMXCan_Private_DlcToBytes(uint32_t dlc);

/**
 * @brief Processes periodic tasks for the CubeMX CAN handle, such as handling timeouts and managing the transmission queue.
 */
void CubeMXCan_Tick(void);

/**
 * @brief Callback function for handling received messages from the FDCAN peripheral. This function is called when a message is received in the RX FIFO 0 of the FDCAN peripheral.
 * @param hfdcan Pointer to the FDCAN handle associated with the received message.
 */
void CubeMXCan_OnRxFifo0(FDCAN_HandleTypeDef *hfdcan);

#endif
