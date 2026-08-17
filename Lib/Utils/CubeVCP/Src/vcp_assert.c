#include <assert.h>

#include "CubeVCP.h"
#include "CubeVCP_Config.h"
#include "main.h"

static_assert(ATOMIC_SHORT_LOCK_FREE == 2, "CubeVCP internal ATOMIC_SHORT_LOCK_FREE must be enabled for atomic operations");
static_assert(ATOMIC_BOOL_LOCK_FREE == 2, "CubeVCP internal ATOMIC_BOOL_LOCK_FREE must be enabled for atomic operations");
static_assert((CUBE_VCP_TX_BUFFER_SIZE > 0) && !(CUBE_VCP_TX_BUFFER_SIZE & (CUBE_VCP_TX_BUFFER_SIZE - 1U)), "CUBE_VCP_TX_BUFFER_SIZE must be a power of two and greater than zero");
