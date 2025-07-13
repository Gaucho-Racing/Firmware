/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include <stdint.h>

#include "utils.h"
#include "CANdler.h"

// FIXME Remove when ready, controls if CAN or no CAN
// #define TEST_WITHOUT_CAN true

FDCAN_TxHeaderTypeDef TxHeader = {
  .IdType = FDCAN_EXTENDED_ID,
  .TxFrameType = FDCAN_DATA_FRAME,
  .FDFormat = FDCAN_CLASSIC_CAN,
  .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
                                           // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
  .BitRateSwitch = FDCAN_BRS_OFF,
  .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
  .MessageMarker = 0 // also change this to a real address if you change fifo control
};

void writeMessage(uint16_t msgID, uint8_t destID, uint8_t data[], uint32_t length)
{
  TxHeader.Identifier = (LOCAL_GR_ID << 20) | (msgID << 8) | destID;
  TxHeader.DataLength = length;

  FDCAN_HandleTypeDef *handle;
  handle = &hfdcan1;

  FDCAN_ProtocolStatusTypeDef protocolStatus = {};
  HAL_FDCAN_GetProtocolStatus(handle, &protocolStatus);
  if (protocolStatus.BusOff)
  {
    CLEAR_BIT(handle->Instance->CCCR, FDCAN_CCCR_INIT);
  }

  if (HAL_FDCAN_GetTxFifoFreeLevel(handle) == 0)
  {
    LOGOMATIC("\nFDCAN Tx FIFO is full\n");
  }
  else if (HAL_FDCAN_AddMessageToTxFifoQ(handle, &TxHeader, data) != HAL_OK)
  {
    LOGOMATIC("\nCould not add msg to transmission FIFO queue\n");
    Error_Handler();
  }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  static FDCAN_RxHeaderTypeDef RxHeader;
  static uint8_t RxData[8];

  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
      if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
      {
          Error_Handler();
      }

      if ((RxHeader.Identifier & ~0xF00) == 0x2016)
      {
          uint8_t temp;
          for (uint16_t i = 0; i < RxHeader.DataLength / 2; ++i) // Because DTI
          {
              temp = RxData[i];
              RxData[i] = RxData[RxHeader.DataLength - i - 1];
              RxData[RxHeader.DataLength - i - 1] = temp;
          }

          handleDtiCANMessage(RxHeader.Identifier, RxData, RxHeader.DataLength);
      }
      else
      {
          handleCANMessage((RxHeader.Identifier & 0x00FFF00) >> 8, (RxHeader.Identifier & 0xFF00000) >> 20, RxData, RxHeader.DataLength);
      }

      if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
      {
          Error_Handler();
      }
  }
}
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 8;
  hfdcan1.Init.NominalSyncJumpWidth = 16;
  hfdcan1.Init.NominalTimeSeg1 = 14;
  hfdcan1.Init.NominalTimeSeg2 = 5;
  hfdcan1.Init.DataPrescaler = 8;
  hfdcan1.Init.DataSyncJumpWidth = 16;
  hfdcan1.Init.DataTimeSeg1 = 14;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 2;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
  FDCAN_FilterTypeDef fdcan1_filter;

  fdcan1_filter.IdType = FDCAN_EXTENDED_ID;
  fdcan1_filter.FilterIndex = 0;
  fdcan1_filter.FilterType = FDCAN_FILTER_MASK;
  fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  fdcan1_filter.FilterID1 = LOCAL_GR_ID; // filter messages with ECU destination
  fdcan1_filter.FilterID2 = 0xFF;

  if(HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan1_filter) != HAL_OK) {
      Error_Handler();
  }

  fdcan1_filter.FilterIndex = 1;
  fdcan1_filter.FilterID1 = 0xFF; // filter messages for all targets

  if(HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan1_filter) != HAL_OK) {
      Error_Handler();
  }

  if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
      Error_Handler();
  }

  HAL_FDCAN_Start(&hfdcan1);
  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN1;
    PeriphClkInit.Fdcan1ClockSelection = RCC_FDCAN1CLKSOURCE_PLL1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PB8     ------> FDCAN1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN1_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PB9     ------> FDCAN1_TX
    PB8     ------> FDCAN1_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9|GPIO_PIN_8);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
