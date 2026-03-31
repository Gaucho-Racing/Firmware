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
#include "stm32g4xx_it.h"

#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CCUStateData.h"
#include "Logomatic.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
/* USER CODE BEGIN EV */

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

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
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

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1) {
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

	/* USER CODE END BusFault_IRQn 0 */
	while (1) {
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

	/* USER CODE END UsageFault_IRQn 0 */
	while (1) {
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

	/* USER CODE END SysTick_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void USART2_IRQHandler(void)
{
	if (LL_USART_IsActiveFlag_ORE(USART2)) {
		LL_USART_ClearFlag_ORE(USART2);
	}
	while (LL_USART_IsEnabledIT_RXNE(USART2) && LL_USART_IsActiveFlag_RXNE(USART2)) {
		uint8_t receivedData = LL_USART_ReceiveData8(USART2);
		while (!LL_USART_IsActiveFlag_TXE_TXFNF(USART2)) {}
		LOGOMATIC("VCP: %c\n", receivedData);
		if (receivedData == 'C' && !state_data.recv_charge_cmd) {
			LOGOMATIC("Received charge command\n");
			state_data.recv_charge_cmd = true;
			LL_USART_TransmitData8(USART2, 'C');
		 }
		else if (receivedData == '?') {

			LOGOMATIC("Received State Dump command\n");
			LOGOMATIC("\n========== CCU STATE DUMP ==========\n");

			LOGOMATIC("state: %d\n", state_data.state);
			LOGOMATIC("recv_charge_cmd: %d\n", state_data.recv_charge_cmd);

			LOGOMATIC("\n--- BCU_STATUS_2 ---\n");
			LOGOMATIC("20V: %u\n", state_data.BCU_S2_20Volt);
			LOGOMATIC("12V: %u\n", state_data.BCU_S2_12Volt);
			LOGOMATIC("SDC Volt: %u\n", state_data.BCU_S2_SDC_Volt);
			LOGOMATIC("Min Cell Volt: %u\n", state_data.BCU_S2_MIN_CELL_Volt);
			LOGOMATIC("Max Cell Temp: %u\n", state_data.BCU_S2_MAX_CELL_TEMP);

			LOGOMATIC("\n--- Errors ---\n");
			LOGOMATIC("OVERTEMP: %d\n", state_data.BCU_S2_OVERTEMP_ERROR);
			LOGOMATIC("OVERVOLT: %d\n", state_data.BCU_S2_OVERVOLT_ERROR);
			LOGOMATIC("UNDERVOLT: %d\n", state_data.BCU_S2_UNDERVOLT_ERROR);
			LOGOMATIC("OVERCURR: %d\n", state_data.BCU_S2_OVERCURR_ERROR);
			LOGOMATIC("UNDERCURR: %d\n", state_data.BCU_S2_UNDERCURR_ERROR);

			LOGOMATIC("\n--- Warnings ---\n");
			LOGOMATIC("UNDER20V: %d\n", state_data.BCU_S2_UNDER20v_WARNING);
			LOGOMATIC("UNDER12V: %d\n", state_data.BCU_S2_UNDER12v_WARNING);
			LOGOMATIC("UNDERVOLT SDC: %d\n", state_data.BCU_S2_UNDERVOLTSDC_WARNING);

			LOGOMATIC("\n--- State Bits ---\n");
			LOGOMATIC("SOFTWARE LATCH: %d\n", state_data.BCU_S2_SOFTWARE_LATCH);
			LOGOMATIC("PRECHARGE TS ACTIVE: %d\n", state_data.BCU_PRECHARGE_SET_TS_ACTIVE);

			LOGOMATIC("====================================\n\n");

			LL_USART_TransmitData8(USART2, '?');

		}
		 else {
			state_data.recv_charge_cmd = false;
			LL_USART_TransmitData8(USART2, 'X');
		}
	}
}
/* USER CODE END 1 */
