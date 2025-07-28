/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
#include <stdio.h>
#include "acu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern volatile uint8_t TPL_RxBuffer[256]; // Array to store received SPI data
extern volatile uint8_t TPL_RxBufferLevel; // Number of bytes to be read
extern volatile uint8_t TPL_RxBufferBottom; // Index of oldest data
extern volatile uint8_t TPL_RxBufferTop; // Index of newest data
volatile uint16_t delaying = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
extern void parse_data();
extern void dequeue_in_irq();
extern void print_lpuart(char* arr);
extern void enqueue(uint32_t id, FDCAN_GlobalTypeDef * which_can);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
/* USER CODE BEGIN EV */
extern volatile uint8_t CAN_RxBufferLevel;
extern volatile uint8_t p_bottom, d_bottom, c_bottom, p_level, d_level, c_level;
extern volatile uint8_t prim_q[256], data_q[256], charger_q[256]; 
extern volatile uint8_t CAN_1_flag;
extern volatile uint8_t CAN_2_flag;
extern volatile uint8_t CAN_3_flag;
extern ACU acu; // global ACU variable
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */
  print_lpuart("NMI_Handler");
  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  print_lpuart("HardFault_Handler");
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  print_lpuart("MemManage_Handler");
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  print_lpuart("BusFault_Handler");
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  print_lpuart("UsageFault_Handler");
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */
  print_lpuart("SVC_Handler");
  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */
  print_lpuart("DebugMon_Handler");
  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */
  print_lpuart("PendSV_Handler");
  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  update_adc_data(&acu);
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles FDCAN1 interrupt 0.
  */
void FDCAN1_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 0 */

  /* USER CODE END FDCAN1_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan1);
  /* USER CODE BEGIN FDCAN1_IT0_IRQn 1 */

  /* USER CODE END FDCAN1_IT0_IRQn 1 */
}

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
  /* USER CODE BEGIN SPI2_IRQn 0 */
  if (TPL_RxBufferLevel == 255U) return;
  /* USER CODE END SPI2_IRQn 0 */
  /* USER CODE BEGIN SPI2_IRQn 1 */
  if (LL_SPI_IsActiveFlag_RXNE(SPI2)){
    TPL_RxBuffer[TPL_RxBufferTop] = LL_SPI_ReceiveData8(SPI2);
    TPL_RxBufferLevel++;
    TPL_RxBufferTop++;
  }
  else{
    print_lpuart("This shouldn't happen...\n");
    return;
  }
  
  /* USER CODE END SPI2_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt, DAC2 and DAC4 channel underrun error interrupts.
  */
void TIM7_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_DAC_IRQn 0 */
  LL_TIM_ClearFlag_UPDATE(TIM7);
  /* USER CODE END TIM7_DAC_IRQn 0 */

  /* USER CODE BEGIN TIM7_DAC_IRQn 1 */
  update_relay_state(&acu);
  // always check if we can send data
  if(CAN_1_flag != 0 || CAN_2_flag != 0 || CAN_3_flag != 0) {dequeue_in_irq();}

  // always check if we can read data => HOLD OFF ON THIS
  if(CAN_RxBufferLevel > 0) {parse_data();}

  if(delaying == 0){
    // queue data to send occasionally
    delaying = 500;
    // enqueue(ACU_Cell_Data_1, FDCAN2);
    // enqueue(ACU_Cell_Data_2, FDCAN2);
    // enqueue(ACU_Cell_Data_3, FDCAN2);
    // enqueue(ACU_Cell_Data_4, FDCAN2);
    // enqueue(ACU_Cell_Data_5, FDCAN2);
    enqueue(ACU_Status_1,FDCAN1);
    enqueue(ACU_Status_2,FDCAN1);
    // enqueue(ACU_Charger_Control, FDCAN3);
  }
  else {delaying--;}

  // occasionally push data to "ready send"
  if (CAN_1_flag == 0 && HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) != 0 && p_level > 0) 
  {
    CAN_1_flag = prim_q[p_bottom];
    p_level--;
    p_bottom++;
  }
  if (CAN_2_flag == 0 && HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) != 0 && d_level > 0) 
  {
    CAN_2_flag = data_q[d_bottom];
    d_level--;
    d_bottom++;
  }
  if (CAN_3_flag == 0 && HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan3) != 0 && c_level > 0) 
  {
    CAN_3_flag = charger_q[c_bottom];
    c_level--;
    c_bottom++;
  }
  /* USER CODE END TIM7_DAC_IRQn 1 */
}

/**
  * @brief This function handles FDCAN2 interrupt 0.
  */
void FDCAN2_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 0 */

  /* USER CODE END FDCAN2_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan2);
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 1 */

  /* USER CODE END FDCAN2_IT0_IRQn 1 */
}

/**
  * @brief This function handles FDCAN3 interrupt 0.
  */
void FDCAN3_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN3_IT0_IRQn 0 */

  /* USER CODE END FDCAN3_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan3);
  /* USER CODE BEGIN FDCAN3_IT0_IRQn 1 */

  /* USER CODE END FDCAN3_IT0_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
