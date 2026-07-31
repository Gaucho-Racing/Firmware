#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN_Config.h"
#include "Logomatic.h"
#include "Stringification.h"
#include "fdcan.h"
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
 * @brief Rx events mask for FDCAN notifications.
 *
 * This mask is used to enable notifications for new messages, full FIFO, and message lost events in the FDCAN peripheral.
 */
#define FDCAN_IT_RX_EVENTS (FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_RX_FIFO0_FULL | FDCAN_IT_RX_FIFO0_MESSAGE_LOST)

/**
 * @brief Transmission CAN message structure
 *
 * This structure is used to represent a transmission CAN message, which consists of a header and data payload. The header contains information about the message, such as its identifier, data length,
 * and other relevant parameters. The data payload contains the actual data bytes of the message.
 *
 * @warning The length of the data array may be longer than the actual CAN message.
 */
typedef struct {
	FDCAN_TxHeaderTypeDef tx_header;
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} GRCAN_Private_TxMessage;

/**
 * @brief CubeMX CAN handle structure
 *
 * This structure is used to represent a CubeMX CAN handle, which consists of a pointer to the FDCAN handle, a configuration structure, a transmission queue, and other relevant parameters.
 *
 * @warning Use the provided API functions to interact with the CubeMX CAN handle.
 * @warning The structure is intended for internal use only and should not be accessed directly by user code.
 */
struct CubeMXCan_Private_Handle {
	/// @brief Pointer to the FDCAN handle associated with this CubeMX CAN handle.
	FDCAN_HandleTypeDef *hfdcan;
	/// @brief Configuration structure for the CubeMX CAN handle, containing the receive callback and user context.
	CubeCAN_Config config;
	/// @brief Transmission queue for the CubeMX CAN handle, containing the messages to be transmitted.
	GRCAN_Private_TxMessage tx_queue[CUBEMX_CAN_TX_QUEUE_SIZE];
	/// @brief Atomic head index for the transmission queue, indicating the next message to be transmitted.
	_Atomic uint32_t tx_head;
	/// @brief Atomic tail index for the transmission queue, indicating the next available slot for a new message.
	_Atomic uint32_t tx_tail;
	/// @brief Flag indicating whether the CubeMX CAN handle has been started.
	bool started;
};

/// @brief Maximum number of CubeMX CAN instances supported by the library.
#ifndef CUBEMX_CAN_MAX_INSTANCES
#if (defined(FDCAN3) || defined(CAN3)) && (defined(FDCAN2) || defined(CAN2)) && (defined(FDCAN1) || defined(CAN1))
#define CUBEMX_CAN_MAX_INSTANCES 3U
#elif (defined(FDCAN2) || defined(CAN2)) && (defined(FDCAN1) || defined(CAN1))
#define CUBEMX_CAN_MAX_INSTANCES 2U
#elif (defined(FDCAN1) || defined(CAN1))
#define CUBEMX_CAN_MAX_INSTANCES 1U
#else
#error "No CAN or FDCAN instances defined. Please check your CubeMX configuration."
#endif
#endif

/**
 * @brief Sends a queued message from the CubeMX CAN handle.
 * @param handle Pointer to the CubeMX CAN handle.
 * @return HAL_StatusTypeDef indicating the success or failure of the operation.
 */
HAL_StatusTypeDef CubeMXCan_Private_SendQueuedMessage(const CubeMXCan_Handle *const handle);

/**
 * @brief Array of CubeMX CAN handles, one for each supported instance.
 * @note The number of instances is defined by CUBEMX_CAN_MAX_INSTANCES.
 */
extern struct CubeMXCan_Private_Handle handles[CUBEMX_CAN_MAX_INSTANCES];

/**
 * @brief Flag indicating whether the CubeMX CAN Tx timer has been started.
 */
extern bool timer_started;

/**
 * @brief Attempts to recover the FDCAN peripheral associated with the given CubeMX CAN handle.
 *
 * Handles recovery from bus off or restricted operation mode by reinitializing the FDCAN peripheral and restoring its configuration.
 *
 * @param handle Pointer to the CubeMX CAN handle associated with the FDCAN peripheral to be recovered.
 * @return HAL_StatusTypeDef indicating the success or failure of the recovery operation.
 */
HAL_StatusTypeDef CubeMXCan_Private_RecoverPeripheral(const CubeMXCan_Handle *const handle);

/**
 * @brief Converts a Data Length Code (DLC) to the corresponding number of data bytes.
 * @param dlc The Data Length Code to be converted.
 * @return The number of data bytes corresponding to the given DLC.
 * @warning The DLC value must be between 0 and 15, inclusive. Values outside this range will result in undefined behavior.
 */
bool CubeMXCan_Private_IsDisabled(const CubeMXCan_Handle *const handle);

/**
 * @brief Queues a transmission message for the CubeMX CAN handle, allowing for asynchronous message transmission.
 * @param handle Pointer to the CubeMX CAN handle.
 * @param message Pointer to the GRCAN transmission message structure.
 * @return HAL_StatusTypeDef if the parameters are non-null or the queue is full.
 */
HAL_StatusTypeDef CubeMXCan_Private_QueueTx(CubeMXCan_Handle *const handle, const GRCAN_Private_TxMessage *const message);

/**
 * @brief Converts a Data Length Code (DLC) to the corresponding number of bytes.
 * @param dlc The Data Length Code to be converted.
 * @return The number of bytes corresponding to the given DLC.
 * @warning The DLC value must be between 0 and 15, inclusive. Values outside this range will result in undefined behavior.
 */
uint8_t CubeMXCan_Private_DlcToBytes(const uint32_t dlc);

/**
 * @brief Converts a number of bytes to the corresponding Data Length Code (DLC).
 * @param bytes The number of bytes to be converted.
 * @return The Data Length Code corresponding to the given number of bytes.
 * @warning The number of bytes must be between 0 and 8, inclusive. Values outside this range will result in undefined behavior.
 */
uint8_t CubeMXCan_Private_BytesToDlc(const uint8_t bytes);

#endif
