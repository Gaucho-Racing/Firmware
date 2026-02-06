#ifndef CANDLER_H
#define CANDLER_H

void CANInitialize();
void CAN_callback(uint32_t ID, void *data, uint32_t size);

#endif
