#include <assert.h>

#include "CubeVCP.h"
#include "CubeVCP_Config.h"
#include "main.h"

static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "CubeVCP internal ATOMIC_BOOL_LOCK_FREE must be enabled for atomic operations");
static_assert(ATOMIC_SHORT_LOCK_FREE == 2, "CubeVCP internal ATOMIC_SHORT_LOCK_FREE must be enabled for atomic operations");

static_assert((CUBE_VCP_TX_BUFFER_SIZE > 0) && !(CUBE_VCP_TX_BUFFER_SIZE & (CUBE_VCP_TX_BUFFER_SIZE - 1U)), "CUBE_VCP_TX_BUFFER_SIZE must be a power of two and greater than zero");
static_assert(CUBE_VCP_RX_BUFFER_SIZE > 8, "CUBE_VCP_RX_BUFFER_SIZE must be greater than 8 bytes to ensure useful command input is possible. It is recommended to set this to at least 32 bytes for practical use cases.");
