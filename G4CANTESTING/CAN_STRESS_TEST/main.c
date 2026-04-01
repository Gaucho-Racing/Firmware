/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

#include "adc.h"
#include "can.h" // Assume this works
#include "can_tests.h"
#include "dma.h"
#include "fdcan.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Logomatic.h"
//#include "profile.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* Enable ITM for SWO output */
static void ITM_Enable(void)
{
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN;

	/* Enable TRC (Trace) */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* Configure TPI for SWO output (set prescaler for 2MHz SWO clock) */
	TPI->SPPR = 2U;	 /* 2 = Manchester/async UART mode */
	TPI->ACPR = 84U; /* Prescaler: (170 MHz / (84+1) / 2) ≈ 1MHz SWO */

	ITM->TER |= (1UL << 0);
	ITM->TCR |= (ITM_TCR_ITMENA_Msk | ITM_TCR_SWOENA_Msk);
}
// static int toggleze = 0;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU
	 * Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the
	 * Systick. */
	HAL_Init();
	/* USER CODE BEGIN Init */
	ITM_Enable();
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	// MX_FDCAN2_Init();
	MX_ADC1_Init();
	MX_LPUART1_UART_Init();
	MX_I2C2_Init();
	MX_USART1_UART_Init();
	MX_SPI3_Init();
	MX_TIM2_Init();

	/* USER CODE BEGIN 2 */

	LOGOMATIC("Booted!\n");

	// LOGOMATIC("running can_stress_test:\n");
	can_stress_test();

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		LOGOMATIC("Main Loop\n");
		LL_mDelay(1000);

		// Receive on GPIOs
		// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, toggleze ? GPIO_PIN_SET
		// : GPIO_PIN_RESET); HAL_Delay(1000); msg.data[0] = toggleze ?
		// 0x00 : 0x80; can_send(data_can, &msg);

		// RCC->CFGR |= RCC_CFGR_SW;
		/* USER CODE BEGIN 3 */
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */

// void SystemClock_Config(void)
// {
// 	LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
// 	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
// 	}
// 	LL_PWR_EnableRange1BoostMode();
// 	LL_RCC_HSE_Enable();

// 	/* Wait till HSE is ready */
// 	while (LL_RCC_HSE_IsReady() != 1) {
// 	}

// 	LL_RCC_HSE_EnableCSS();
// 	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 20,
// 				    LL_RCC_PLLR_DIV_2);
// 	//LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, );
// 	LL_RCC_PLL_EnableDomain_SYS();
// 	LL_RCC_PLL_Enable();
// 	/* Wait till PLL is ready */
// 	while (LL_RCC_PLL_IsReady() != 1) {
// 	}

// 	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
// 	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
// 	/* Wait till System clock is ready */
// 	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
// 	}

// 	/* Insure 1us transition state at intermediate medium speed clock*/
// 	for (__IO uint32_t i = (170 >> 1); i != 0; i--)
// 		;

// 	/* Set AHB prescaler*/
// 	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
// 	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
// 	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
// 	LL_SetSystemCoreClock(160000000);

// 	/* Update the time base */
// 	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
// 		Error_Handler();
// 	}
// }*/
void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {}
	LL_PWR_EnableRange1BoostMode();
	LL_RCC_HSI_Enable();
	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {}

	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 85, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();
	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1) {}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	// LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

	/* Insure 1us transition state at intermediate medium speed clock*/
	for (__IO uint32_t i = (170 >> 1); i != 0; i--)
		;

	/* Set AHB prescaler*/
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_SetSystemCoreClock(170000000);

	/* Update the time base */
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return
	 * state */
	__disable_irq();
	while (1) {}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line
	   number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
	   file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
