#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN_Config.h"
#include "CubeMXCan.h"
#include "Logomatic.h"
#include "Stringification.h"
#include "main.h"

#ifndef PRIVATE_CUBE_MX_CAN_H
#define PRIVATE_CUBE_MX_CAN_H

/**
 * @brief Mask for the transmission queue index, used to wrap around the queue when it reaches its maximum size.
 *
 * Application code should not use this macro directly. It is intended for internal use only
 *
 * @warning The transmission queue size must be a power of two for this mask to work correctly
 * @warning The transmission queue size must be defined as CUBEMX_CAN_TX_QUEUE_SIZE in the CubeMX CAN configuration header file
 */
#define TX_QUEUE_MASK (CUBEMX_CAN_TX_QUEUE_SIZE - 1U)

/**
 * @brief Function to convert a CubeMX CAN instance pointer to its corresponding index.
 *
 * Works on all supported boards, but new boards should be verified against this operation
 *
 * @param can Pointer to the CubeMX CAN instance (FDCAN1, FDCAN2, or FDCAN3).
 * @return The index corresponding to the given CubeMX CAN instance (0 for FDCAN1, 1 for FDCAN2, and 2 for FDCAN3).
 */
uint32_t CubeMXCan_Private_InstanceToIndex(FDCAN_GlobalTypeDef *can);

/**
 * @brief CubeMX CAN handle structure
 *
 * This structure is used to represent a CubeMX CAN handle, which consists of a pointer to the FDCAN handle, a configuration structure, a transmission queue, and other relevant parameters.
 *
 * @warning Use the provided API functions to interact with the CubeMX CAN handle.
 * @warning The structure is intended for internal use only and should not be accessed directly by user code.
 */
struct CubeMXCan_Handle {
	/// @brief Pointer to the FDCAN handle associated with this CubeMX CAN handle.
	FDCAN_HandleTypeDef *hfdcan;
	/// @brief Configuration structure for the CubeMX CAN handle, containing the receive callback and user context.
	CubeCAN_Config config;
	/// @brief Transmission queue for the CubeMX CAN handle, containing the messages to be transmitted.
	GRCAN_TxMessage tx_queue[CUBEMX_CAN_TX_QUEUE_SIZE];
	/// @brief Atomic head index for the transmission queue, indicating the next message to be transmitted.
	_Atomic uint32_t tx_head;
	/// @brief Atomic tail index for the transmission queue, indicating the next available slot for a new message.
	_Atomic uint32_t tx_tail;
	/// @brief Flag indicating whether the CubeMX CAN handle has been started.
	bool started;
};

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

/**
 * @brief Structure representing a registry entry for the CubeMX CAN library, used to associate a CubeMX CAN handle with its corresponding FDCAN handle.
 */
struct CubeMXCan_RegistryEntry {
	/// @brief Pointer to the FDCAN instance associated with this registry entry.
	FDCAN_GlobalTypeDef *instance;
	/// @brief Pointer to the CubeMX CAN handle associated with this registry entry.
	CubeMXCan_Handle *handle;
};

/**
 * @brief Registry entry structure for the CubeMX CAN library, used to associate a CubeMX CAN handle with its corresponding FDCAN handle.
 * @note The number of entries in the registry is defined by CUBEMX_CAN_MAX_INSTANCES.
 */
extern struct CubeMXCan_RegistryEntry registry[CUBEMX_CAN_MAX_INSTANCES];

/**
 * @brief Array of CubeMX CAN handles, one for each supported instance.
 * @note The number of instances is defined by CUBEMX_CAN_MAX_INSTANCES.
 */
extern struct CubeMXCan_Handle handles[CUBEMX_CAN_MAX_INSTANCES];

/**
 * @brief Flag indicating whether the CubeMX CAN Tx timer has been started.
 */
extern bool s_timer_started;

/**
 * @brief Attempts to recover the FDCAN peripheral associated with the given CubeMX CAN handle.
 *
 * Handles recovery from bus off or restricted operation mode by reinitializing the FDCAN peripheral and restoring its configuration.
 *
 * @param handle Pointer to the CubeMX CAN handle associated with the FDCAN peripheral to be recovered.
 * @return HAL_StatusTypeDef indicating the success or failure of the recovery operation.
 */
HAL_StatusTypeDef CubeMXCan_Private_RecoverPeripheral(CubeMXCan_Handle *handle);

#endif
