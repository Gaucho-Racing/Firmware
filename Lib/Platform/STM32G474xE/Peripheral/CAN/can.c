#include "../../Utils/ICANStruct/ICANStruct.h"

// External references for the MX init functions
extern void MX_FDCAN1_Init(void);
extern void MX_FDCAN2_Init(void);
extern void MX_FDCAN3_Init(void);

// External FDCAN handles
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

// Generic functions that use the handle from the struct
static inline uint8_t can_start(CAN *self) {
    return HAL_FDCAN_Start(&self->hfdcan) == HAL_OK ? 1 : 0;
}

static inline uint8_t can_activate_notifications(CAN *self) {
    return HAL_FDCAN_ActivateNotification(&self->hfdcan, 0x01, 0) == HAL_OK ? 1 : 0;
}

static inline uint8_t can_send(CAN *self) {
    // Use self->hfdcan for the actual send implementation
    return 1;
}

// Only the MX init functions need to be specific per CAN instance
static inline uint8_t can1_mx_init(void) {
    MX_FDCAN1_Init();
    return 1;
}

static inline uint8_t can2_mx_init(void) {
    MX_FDCAN2_Init();
    return 1;
}

static inline uint8_t can3_mx_init(void) {
    MX_FDCAN3_Init();
    return 1;
}

static inline uint8_t can_init(CAN *self){
    self->c_mx_init;
    self->c_activate_notification;
    self->c_start;
    return 1;
}

CAN CAN1 = {
    .hfdcan = {0}, 
    .c_mx_init = can1_mx_init,
    .c_start = can_start,
    .c_activate_notification = can_activate_notifications,
    .init = can_init,
    .send = can_send,
    .recieve = CAN_Receive
};

CAN CAN2 = {
    .hfdcan = {0},
    .c_mx_init = can2_mx_init,
    .c_start = can_start,
    .c_activate_notification = can_activate_notifications,
    .init = can_init,
    .send = can_send,
    .recieve = CAN_Receive
};

CAN CAN3 = {
    .hfdcan = {0}, 
    .c_mx_init = can3_mx_init,
    .c_start = can_start,
    .c_activate_notification = can_activate_notifications,
    .init = can_init,
    .send = can_send,
    .recieve = CAN_Receive
};