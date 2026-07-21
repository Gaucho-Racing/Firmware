#include <assert.h>
#include <stdatomic.h>

#include "CriticalSection.h"
#include "CubeCAN_Config.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Private/PrivateCubeMXCAN.h"
#include "main.h"

#pragma region Platform Assertions

static_assert(sizeof(((CubeMXCan_Handle *)0)->tx_head) == sizeof(uint32_t), "tx_head must be 32 bits");

static_assert(sizeof(GRCAN_TxMessage) == (sizeof(FDCAN_TxHeaderTypeDef) + 64), "GRCAN_TxMessage size is incorrect");

static_assert(_Alignof(GRCAN_TxMessage) >= 4, "GRCAN_TxMessage must have at least 4-byte alignment");

static_assert(ATOMIC_INT_LOCK_FREE == 2, "ATOMIC_INT_LOCK_FREE must be enabled for atomic operations on tx_head and tx_tail");

#pragma endregion
#pragma region Configuration Validations

static_assert((CUBEMX_CAN_TX_QUEUE_SIZE > 0) && !(CUBEMX_CAN_TX_QUEUE_SIZE & (CUBEMX_CAN_TX_QUEUE_SIZE - 1)), "CUBEMX_CAN_TX_QUEUE_SIZE must be a power of two and greater than zero");

static_assert(CUBEMX_CAN_MAX_INSTANCES >= 1 && CUBEMX_CAN_MAX_INSTANCES <= 3, "CUBEMX_CAN_MAX_INSTANCES must be configured between 1 and 3 inclusive");

#pragma endregion
