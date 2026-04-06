
#ifndef CAN_DMA_H
#define CAN_DMA_H

#include <stdint.h>

#include "can.h"
#include "can_platform_deps.h"

HAL_StatusTypeDef FDCAN_GetRxMessage_DMA(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation, FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData);
void DMA_M2M_Init(uint32_t preempt, uint32_t subpriority, CAN_RXCallback callback);

#define RxFifoElementsNumber 3
// bool is_valid_rxfifo0_address(FDCAN_HandleTypeDef *hfdcan, uint32_t *RxAddress);

#endif
