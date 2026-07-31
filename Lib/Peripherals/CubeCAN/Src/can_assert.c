#include <assert.h>
#include <stdalign.h>
#include <stdatomic.h>
#include <stddef.h>

#include "CriticalSection.h"
#include "CubeCAN_Config.h"
#include "CubeMXCan.h"
#include "PrivateInc/internal.h"
#include "main.h"

#pragma region Compiler Assertions

static_assert(sizeof(((struct CubeMXCan_Private_Handle *)0)->tx_head) == sizeof(uint32_t), "CubeMXCan internal tx_head must be 32 bits");
static_assert(sizeof(((struct CubeMXCan_Private_Handle *)0)->tx_tail) == sizeof(uint32_t), "CubeMXCan internal tx_tail must be 32 bits");

static_assert(alignof(GRCAN_Private_TxMessage) >= 4, "CubeMXCan internal GRCAN_Private_TxMessage must have at least 4-byte alignment");
static_assert(alignof(struct CubeMXCan_Private_Handle) >= 4, "CubeMXCan internal CubeMXCan_Private_Handle must have at least 4-byte alignment");

static_assert((offsetof(struct CubeMXCan_Private_Handle, tx_head) % 4U) == 0U, "CubeMXCan internal atomic tx_head is unaligned");
static_assert((offsetof(struct CubeMXCan_Private_Handle, tx_tail) % 4U) == 0U, "CubeMXCan internal atomic tx_tail is unaligned");

static_assert(ATOMIC_INT_LOCK_FREE == 2, "CubeMXCan internal ATOMIC_INT_LOCK_FREE must be enabled for atomic operations");
static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "CubeMXCan internal ATOMIC_BOOL_LOCK_FREE must be enabled for atomic operations");

#pragma endregion

#pragma region Configuration Checks

static_assert((CUBEMX_CAN_TX_QUEUE_SIZE > 0) && !(CUBEMX_CAN_TX_QUEUE_SIZE & (CUBEMX_CAN_TX_QUEUE_SIZE - 1U)), "CUBEMX_CAN_TX_QUEUE_SIZE must be a power of two and greater than zero");
static_assert(CUBEMX_CAN_MAX_INSTANCES >= 1 && CUBEMX_CAN_MAX_INSTANCES <= 3, "CUBEMX_CAN_MAX_INSTANCES must be configured between 1 and 3 inclusive");

#pragma endregion
