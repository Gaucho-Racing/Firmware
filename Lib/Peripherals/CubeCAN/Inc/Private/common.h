#include "main.h"

#ifndef CUBEMX_CAN_INTERNAL_H
#define CUBEMX_CAN_INTERNAL_H

/**
 * @brief Get the name of the FDCAN instance as a string.
 *
 * @param instance Pointer to the FDCAN instance.
 *
 * @return A string representing the name of the FDCAN instance. If the instance is not recognized, returns "UNKNOWN".
 */
const char *CAN_GetInstanceName(FDCAN_GlobalTypeDef *instance);

#endif
