#include "CCUStateData.h"
#ifndef CANDLER_H
#define CANDLER_H
void CAN_Configure();
void SendPrechargeStatus(CCU_StateData* state_data);
void SendDebugReport(char* data);
#endif
