#include <stdint.h>

#include "GR_OLD_MSG_ID.h"
#include "GR_OLD_NODE_ID.h"

#ifndef GRCAN_UTILS_H
#define GRCAN_UTILS_H

uint32_t FDCAN_DLC_BYTES(uint32_t size);
uint32_t FDCAN_DLC_SIZE(uint32_t dlc);

#endif
