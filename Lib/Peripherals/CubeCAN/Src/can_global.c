#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "CriticalSection.h"
#include "CubeMXCan.h"
#include "CubeMXCanExt.h"
#include "Private/PrivateCubeMXCAN.h"

struct CubeMXCan_Private_Handle handles[CUBEMX_CAN_MAX_INSTANCES] = {0};
bool timer_started = false;
