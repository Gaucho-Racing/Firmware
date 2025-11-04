#include "StateUtils.h"

uint8_t CriticalError(ECU_StateData *stateData){
    if (stateData->MaxCellTemp > 60){
        return 1;
    }
    if (stateData->TractiveSystemVoltage > 600) {
        return 1;
    }

}
uint8_t CommunicationError(ECU_StateData *stateData);
uint8_t APPS_BSE_Violation();