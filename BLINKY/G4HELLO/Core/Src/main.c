/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Logomatic.h"
#include "Peripherals/USART/usart.h"
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
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void received_byte_callback(uint8_t byte)
{
	LOGOMATIC("Received byte: 0x%02X ('%c')\n", byte, (byte >= 32 && byte <= 126) ? byte : '.');

#ifndef LOGOMATIC_ENABLED
	byte = byte; // suppress unused variable warning
#endif
}

/* Enable ITM for SWO output */
static void ITM_Enable(void)
{
	/* Enable TRC (Trace) */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	/* Enable stimulus port 0 */
	ITM->TER |= (1UL << 0);

	/* Set trace control register */
	ITM->TCR |= ITM_TCR_ITMENA_Msk;
}

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
	// MX_LPUART1_UART_Init();
	/* USER CODE BEGIN 2 */
	USARTConfig config = {
	    .instance = LPUART1,
	    .tx_queue_length = 16,
	    .on_rx_byte = NULL,
	    .gpio_port = GPIOA,
	    .ll_gpio =
		&(LL_GPIO_InitTypeDef){
		    .Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
		    .Mode = LL_GPIO_MODE_ALTERNATE,
		    .Alternate = LL_GPIO_AF_12,
		    .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
		    .Pull = LL_GPIO_PULL_NO,
		    .Speed = LL_GPIO_SPEED_FREQ_LOW,
		},
	    .ll_lpuart =
		&(LL_LPUART_InitTypeDef){
		    .PrescalerValue = LL_LPUART_PRESCALER_DIV1,
		    .BaudRate = 115200,
		    .DataWidth = LL_LPUART_DATAWIDTH_8B,
		    .StopBits = LL_LPUART_STOPBITS_1,
		    .Parity = LL_LPUART_PARITY_NONE,
		    .TransferDirection = LL_LPUART_DIRECTION_TX_RX,
		},
	};
	USARTHandle *handle = usart_init_peripheral(&config);
	for (uint8_t i = 0; i < 30; i++) {
		LOGOMATIC("sending message over usart... %d\n", i);
		usart_send(handle, "Hello World!\r\n", 14);
		LL_mDelay(1000);
	}
	LOGOMATIC("USART test messages sent, releasing...\n");
	usart_release(&handle);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		LOGOMATIC("Hello, LOGOMATIC! Great to be here %f\n", 3.14159265);

		LL_mDelay(750);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
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
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
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

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
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
