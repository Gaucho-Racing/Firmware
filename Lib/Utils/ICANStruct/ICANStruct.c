#include "ICANStruct.h"

__weak uint8_t CAN_MX_Init(void) {
    return 0;
}

__weak uint8_t CAN_Start(void) {
    return 0;
}

__weak uint8_t CAN_Activate_Notification(void) {
    return 0;
}

__weak uint8_t CAN_Init(void) {
    return 0;
}

__weak uint8_t CAN_Send(void) {
    return 0;
}

__weak uint8_t CAN_Receive(void) {
    return 0;
}

uint8_t RX_dequeue(CAN *self, FDCAN_RxHeaderTypeDef *header, uint8_t* data) {
    return 0;
}

uint8_t RX_enqueue(CAN *self, FDCAN_RxHeaderTypeDef *header, uint8_t* data) {
    return 0;
}

uint8_t TX_dequeue(CAN *self, FDCAN_TxHeaderTypeDef *header, uint8_t* data) {
    return 0;
}

uint8_t TX_enqueue(CAN *self, FDCAN_TxHeaderTypeDef *header, uint8_t* data) {
    return 0;
}

void CAN_InitStruct(CAN *can_instance) {
    if (can_instance == NULL) return;
    
    can_instance->c_mx_init = CAN_MX_Init;
    can_instance->c_start = CAN_Start;
    can_instance->c_activate_notification = CAN_Activate_Notification;
    can_instance->init = CAN_Init;
    can_instance->send = CAN_Send;
    can_instance->recieve = CAN_Receive;
}