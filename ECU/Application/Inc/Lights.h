#ifndef LIGHTS_H_
#define LIGHTS_H_

#include "StateData.h"

/**
 * @brief Event handler for hardware lights.
 *
 * Controls all lights managed by ECU; calls light helper functions.
 *
 * @param stateData Pointer to the ECU_StateData structure.
 *
 * @return void
 */
void lightControl(ECU_StateData *stateData);

#endif
