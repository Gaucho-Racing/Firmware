#include <stdint.h>
#include "StateData.h"

#ifndef STATE_DATA_PARSER_H
#define STATE_DATA_PARSER_H

// Checks stateData for critical errors
uint8_t CriticalError(ECU_StateData *stateData);
uint8_t CommunicationError(ECU_StateData *stateData);
uint8_t APPS_BSE_Violation();

#endif