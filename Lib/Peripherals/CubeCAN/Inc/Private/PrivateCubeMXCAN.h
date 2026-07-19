#include <cmsis_compiler.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeMXCan.h"
#include "Stringification.h"
#include "main.h"

#ifndef PRIVATE_CUBE_MX_CAN_H
#define PRIVATE_CUBE_MX_CAN_H

/**
 * @brief Internal function to automatically restore the interrupt state when exiting a critical section.
 *
 * This function is intended to be used with the GCC cleanup attribute to automatically restore the interrupt state when a critical section is exited. It takes a pointer to a state variable that holds
 * the previous interrupt state and restores it using the __set_PRIMASK function.
 *
 * @warning This function is intended for internal use only and should not be called directly by user code. Use the CRITICAL_SECTION macro instead.
 */
static inline void _magic_auto_exit_critical(uint32_t *state_var)
{
	if (state_var) {
		__set_PRIMASK(*state_var);
	}
}

/**
 * @brief Macro to create a critical section that disables interrupts for the duration of the block.
 *
 * This macro uses the GCC cleanup attribute to automatically restore the interrupt state when the block is exited, regardless of how the block is exited (e.g., return, break, continue).
 *
 * @warning This macro should be used with caution, as it can lead to deadlocks or other issues if not used correctly. It is recommended to use this macro only in situations where it is necessary to
 * disable interrupts for a short period of time.
 */
#define CRITICAL_SECTION                                                                                                                                                                               \
	for (__attribute__((cleanup(_magic_auto_exit_critical))) uint32_t CONCAT(auto_state_, __LINE__) = __get_PRIMASK(), CONCAT(auto_run_, __LINE__) = (__disable_irq(), 1);                         \
	     CONCAT(auto_run_, __LINE__); CONCAT(auto_run_, __LINE__) = 0)

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
