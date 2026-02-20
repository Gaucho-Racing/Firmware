#define LIGHTS_H_
#ifndef LIGHTS_H_

#include "StateData.h"

void brakeLights(ECU_StateData* stateLump);

void TSSILight(ECU_StateData* stateLump);

void RTDButtonLight(ECU_StateData* stateLump);

void TSActiveButtonLight(ECU_StateData* stateLump);

#endif
