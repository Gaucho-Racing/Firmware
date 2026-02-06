#ifndef CANDLER_H
#define CANDLER_H

typedef struct {
    uint16_t vehicleSpeed;
    uint8_t ECUState;
} DashStatus;
typedef struct {
    uint16_t vehicleSpeed;
    uint8_t ECUState;
} CAN_MSG_ECU;

typedef struct {

} CAN_MSG_PING;

extern DashStatus dashStatus;

void CANInitialize();
void CAN_callback(uint32_t ID, void *data, uint32_t size);

#endif
