#include <stdint.h>

#include "main.h"

// Values do not mean anything in this context and should NOT be used outside of this specific test context

#ifndef FDCAN_H
#define FDCAN_H

#define FDCAN_DLC_BYTES_0 ((uint32_t)0x00000000U)
#define FDCAN_DLC_BYTES_1 ((uint32_t)0x00000001U)
#define FDCAN_DLC_BYTES_2 ((uint32_t)0x00000002U)
#define FDCAN_DLC_BYTES_3 ((uint32_t)0x00000003U)
#define FDCAN_DLC_BYTES_4 ((uint32_t)0x00000004U)
#define FDCAN_DLC_BYTES_5 ((uint32_t)0x00000005U)
#define FDCAN_DLC_BYTES_6 ((uint32_t)0x00000006U)
#define FDCAN_DLC_BYTES_7 ((uint32_t)0x00000007U)
#define FDCAN_DLC_BYTES_8 ((uint32_t)0x00000008U)
#define FDCAN_DLC_BYTES_12 ((uint32_t)0x00000009U)
#define FDCAN_DLC_BYTES_16 ((uint32_t)0x0000000AU)
#define FDCAN_DLC_BYTES_20 ((uint32_t)0x0000000BU)
#define FDCAN_DLC_BYTES_24 ((uint32_t)0x0000000CU)
#define FDCAN_DLC_BYTES_32 ((uint32_t)0x0000000DU)
#define FDCAN_DLC_BYTES_48 ((uint32_t)0x0000000EU)
#define FDCAN_DLC_BYTES_64 ((uint32_t)0x0000000FU)

#define FDCAN_CCCR_FDOE_Pos (8U)
#define FDCAN_CCCR_FDOE_Msk (0x1UL << FDCAN_CCCR_FDOE_Pos)
#define FDCAN_CCCR_FDOE FDCAN_CCCR_FDOE_Msk
#define FDCAN_CCCR_BRSE_Pos (9U)
#define FDCAN_CCCR_BRSE_Msk (0x1UL << FDCAN_CCCR_BRSE_Pos)
#define FDCAN_CCCR_BRSE FDCAN_CCCR_BRSE_Msk

#define FDCAN_FRAME_CLASSIC ((uint32_t)0x00000000U)
#define FDCAN_FRAME_FD_NO_BRS ((uint32_t)FDCAN_CCCR_FDOE)
#define FDCAN_FRAME_FD_BRS ((uint32_t)(FDCAN_CCCR_FDOE | FDCAN_CCCR_BRSE))

#define FDCAN_IE_RF0NE_Pos (0U)
#define FDCAN_IE_RF0NE_Msk (0x1UL << FDCAN_IE_RF0NE_Pos)
#define FDCAN_IE_RF0NE FDCAN_IE_RF0NE_Msk
#define FDCAN_IE_RF0FE_Pos (1U)
#define FDCAN_IE_RF0FE_Msk (0x1UL << FDCAN_IE_RF0FE_Pos)
#define FDCAN_IE_RF0FE FDCAN_IE_RF0FE_Msk
#define FDCAN_IE_RF0LE_Pos (2U)
#define FDCAN_IE_RF0LE_Msk (0x1UL << FDCAN_IE_RF0LE_Pos)
#define FDCAN_IE_RF0LE FDCAN_IE_RF0LE_Msk

#define FDCAN_IT_RX_FIFO0_MESSAGE_LOST FDCAN_IE_RF0LE
#define FDCAN_IT_RX_FIFO0_FULL FDCAN_IE_RF0FE
#define FDCAN_IT_RX_FIFO0_NEW_MESSAGE FDCAN_IE_RF0NE

#define FDCAN_CLASSIC_CAN ((uint32_t)0x00000000U)
#define FDCAN_FD_CAN ((uint32_t)0x00200000U)

#define FDCAN_BRS_OFF ((uint32_t)0x00000000U)
#define FDCAN_BRS_ON ((uint32_t)0x00100000U)

#define FDCAN_ESI_ACTIVE ((uint32_t)0x00000000U)
#define FDCAN_ESI_PASSIVE ((uint32_t)0x80000000U)

#define FDCAN_EXTENDED_ID ((uint32_t)0x00000001U)
#define FDCAN_NO_TX_EVENTS ((uint32_t)0x00000000U)
#define FDCAN_DATA_FRAME ((uint32_t)0x00000000U)
#define FDCAN_RX_FIFO0 ((uint32_t)0x00000000U)

typedef enum {
	HAL_FDCAN_STATE_RESET = 0x00U,
	HAL_FDCAN_STATE_READY = 0x01U,
	HAL_FDCAN_STATE_BUSY = 0x02U,
	HAL_FDCAN_STATE_ERROR = 0x03U
} HAL_FDCAN_StateTypeDef;

typedef struct {
	uint32_t FrameFormat;
} FDCAN_InitTypeDef;

typedef struct {
	uint32_t Identifier;
	uint32_t IdType;
	uint32_t TxFrameType;
	uint32_t DataLength;
	uint32_t ErrorStateIndicator;
	uint32_t BitRateSwitch;
	uint32_t FDFormat;
	uint32_t TxEventFifoControl;
	uint32_t MessageMarker;
} FDCAN_TxHeaderTypeDef;

typedef struct {
	uint32_t Identifier;
	uint32_t IdType;
	uint32_t RxFrameType;
	uint32_t DataLength;
} FDCAN_RxHeaderTypeDef;

typedef struct {
	uint32_t BusOff;
} FDCAN_ProtocolStatusTypeDef;

typedef struct FDCAN_HandleTypeDef {
	FDCAN_InitTypeDef Init;
	HAL_FDCAN_StateTypeDef State;
	FDCAN_ProtocolStatusTypeDef ProtocolStatus;
} FDCAN_HandleTypeDef;

HAL_StatusTypeDef HAL_FDCAN_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t interrupts, uint32_t buffer);
HAL_StatusTypeDef HAL_FDCAN_DeactivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t interrupts);
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_Init(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, const FDCAN_TxHeaderTypeDef *header, const uint8_t *data);
uint32_t HAL_FDCAN_GetTxFifoFreeLevel(FDCAN_HandleTypeDef *hfdcan);
uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *hfdcan, uint32_t fifo);
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t fifo, FDCAN_RxHeaderTypeDef *header, uint8_t *data);
HAL_FDCAN_StateTypeDef HAL_FDCAN_GetState(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_GetProtocolStatus(FDCAN_HandleTypeDef *hfdcan, FDCAN_ProtocolStatusTypeDef *status);
uint32_t HAL_FDCAN_IsRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ExitRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan);

#endif
