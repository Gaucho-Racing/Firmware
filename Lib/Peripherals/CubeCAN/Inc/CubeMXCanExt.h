#include <stdint.h>

#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "main.h"

#ifndef CUBEMX_CAN_EXT_H
#define CUBEMX_CAN_EXT_H

/**
 * @brief CAN Identifier structure
 *
 * This structure is used to represent a CAN message identifier, which consists of a transmitting node ID, a receiving node ID, and a message ID. The structure is used in conjunction with the
 * Construct_Message_ID and Deconstruct_Message_ID functions to convert between the structure representation and the 32-bit integer representation of the CAN message identifier.
 *
 * @warning The structure does not represent cusotm IDs.
 * @warning The structure does not represent actual bit depth
 */
typedef struct {
	GRCAN_NODE_ID tx_node_id;
	GRCAN_NODE_ID rx_node_id;
	GRCAN_MSG_ID msg_id;
} CAN_Identifier;

/**
 * @brief Constructs a CAN message identifier from the given transmitting node ID, receiving node ID, and message ID.
 *
 * @param identifier Pointer to the CAN_Identifier structure containing the node and message IDs.
 *
 * @return The constructed 29-bit CAN message extended identifier.
 */
uint32_t Construct_CAN_Identifier(const CAN_Identifier *identifier);

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
CAN_Identifier Deconstruct_CAN_Identifier(uint32_t message_id);

/**
 * @brief Builds an exact-match extended-ID filter for a given CAN identifier.
 */
HAL_StatusTypeDef CubeCANExt_BuildExtendedFilter(const CAN_Identifier *identifier, uint32_t filter_index, uint32_t fifo, FDCAN_FilterTypeDef *filter);

#endif
