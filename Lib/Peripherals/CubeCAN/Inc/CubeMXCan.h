#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CubeCAN_Config.h"
#include "CubeMXCanExt.h"
#include "main.h"

#ifndef CUBEMX_CAN_H
#define CUBEMX_CAN_H

#define FDCAN_MAX_DATA_BYTES 64U

/**
 * @brief Received CAN message structure
 *
 * This structure is used to represent a received CAN message, which consists of a header and data payload. The header contains information about the message, such as its identifier, data length, and
 * other relevant parameters. The data payload contains the actual data bytes of the message.
 *
 * @warning The length of the data array may be longer than the actual CAN message.
 */
typedef struct {
	/// @brief The header of the received CAN message, containing information such as the identifier, data length, and other relevant parameters.
	FDCAN_RxHeaderTypeDef rx_header;
	/// @brief The data payload of the received CAN message, containing the actual data bytes of the message. The length of this array may be longer than the actual CAN message.
	uint8_t data[FDCAN_MAX_DATA_BYTES];
} GRCAN_RxMessage;

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
} GRCAN_TxMessage;

/// @brief CAN handle for CubeMX CAN. @warning Do not access directly, use the provided API functions.
typedef struct CubeMXCan_Handle CubeMXCan_Handle;

/// @brief Callback function type for receiving CAN messages. @warning Do not call directly, use the provided API functions.
typedef void (*CubeCAN_RxCallback)(const void *const user_ctx, const CAN_Identifier *const identifier, const uint8_t *const data, const uint8_t size);

/**
 * @brief Configuration structure for CubeMX CAN.
 *
 * This structure is used to configure the CubeMX CAN handle, including the receive callback function and user context. The receive callback function is called when a CAN message is received, and the
 * user context is a pointer to user-defined data that can be passed to the callback function.
 *
 * @warning Do not edit after initialization, as it may lead to undefined behavior.
 */
typedef struct {
	/// @brief Callback function for receiving CAN messages. This function is called when a CAN message is received.
	CubeCAN_RxCallback rx_callback;
	/// @brief User-defined context pointer that can be passed to the receive callback function. This can be used to pass additional data or state information to the callback function.
	void *user_ctx;
} CubeCAN_Config;

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
 * @brief Queues a transmission message for the CubeMX CAN handle, allowing for asynchronous message transmission.
 * @param handle Pointer to the CubeMX CAN handle.
 * @param message Pointer to the GRCAN transmission message structure.
 * @return HAL_StatusTypeDef if the parameters are non-null or the queue is full.
 */
HAL_StatusTypeDef CubeMXCan_QueueTx(CubeMXCan_Handle *const handle, const GRCAN_TxMessage *const message);

/**
 * @brief Processes periodic tasks for the CubeMX CAN handle, such as handling timeouts and managing the transmission queue.
 * @warning This function sends one can message per configured bus per call. Not calling it will simply not send any messages.
 */
void CubeMXCan_Tick(void);

#endif
