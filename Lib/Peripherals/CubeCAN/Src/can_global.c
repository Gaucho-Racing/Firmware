#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeCAN.h"
#include "PrivateInc/internal.h"

struct CubeCAN_Private_Handle handles[CUBEMX_CAN_MAX_INSTANCES] = {0};

int five = sizeof(handles);
