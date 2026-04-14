/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32g4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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

#include "CANdler.h"   // For CAN stuff
#include "Logomatic.h" // For Logomatic
#include "bitManipulations.h"
#include "stm32g4xx_ll_gpio.h"
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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
	timer_inc();
	/* USER CODE END SysTick_IRQn 0 */

	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
/**
 * @brief EXTI Line[15:10] Interrupt Handler (for PC13 button)
 * @param None
 * @retval None
 */
// void EXTI15_10_IRQHandler(void)
// {
// 	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13)) {
// 		LOGOMATIC("PC13 Button Pressed!\n");
// 		canReadyToSend = true;
// 		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
// 		LL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
// 	}
// }

/**
 * @brief EXTI Line 3 Interrupt Handler (for TS Active button)
 * @param None
 * @retval None
 */
void EXTI3_IRQHandler(void)
{

	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3)) {

		// Blame Electronics if hardware debounce doesn't work
		SetBitInByte(dashStatus.button_flags, 0, true);
		// dashStatus = 1;
		canReadyToSend = true;
		// LOGOMATIC("TS Active Pressed!");
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
	}
}

/**
 * @brief EXTI Line 4 Interrupt Handler (for RTD button)
 * @param None
 * @retval None
 */
void EXTI4_IRQHandler(void)
{

	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_4)) {

		// Blame Electronics if hardware debounce doesn't work

		if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_4)){
			// A4 Triggered
			SetBitInByte(dashStatus.button_flags, 1, true);
			canReadyToSend = true;
			LOGOMATIC("RTD Pressed!");
		}

		if (LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_4)) {
			// C4 Triggered
			SetBitInByte(dashStatus.button_flags, 5, true);
			canReadyToSend = true;
			LOGOMATIC("Button 4 Pressed!");
		}

		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
	}
}

void EXTI9_5_IRQHandler(void)
{

	// EXTI 5
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5)) {

		SetBitInByte(dashStatus.button_flags, 2, true);
		canReadyToSend = true;
		// LOGOMATIC("Button 1 Pressed!");

		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
	}

	// EXTI 6
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6)) {

		SetBitInByte(dashStatus.button_flags, 3, true);
		canReadyToSend = true;
		// LOGOMATIC("Button 2 Pressed!");

		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
	}

	// EXTI 7
	if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7)) {

		SetBitInByte(dashStatus.button_flags, 4, true);
		canReadyToSend = true;
		// LOGOMATIC("Button 3 Pressed!");

		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
	}
}

/* USER CODE END 1 */
