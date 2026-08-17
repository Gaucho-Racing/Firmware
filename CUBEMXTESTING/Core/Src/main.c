/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"

#include "fdcan.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <inttypes.h>

#include "CANdler.h"
#include "CubeCAN.h"
#include "CubeVCP.h"
#include "GRCAN_BUS_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "Loop.h"
#include "tim.h"
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
CubeCAN_Handle *primaryHandle = NULL;
CubeCAN_Handle *dataHandle = NULL;
CubeCAN_Handle *chargerHandle = NULL;
UART_HandleTypeDef *const vcp_uart_handle = &hlpuart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	Logomatic_SetLogLevel(LogLevel_Debug);
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_FDCAN1_Init();
	MX_FDCAN2_Init();
	MX_TIM5_Init();
	MX_FDCAN3_Init();
	MX_LPUART1_UART_Init();
	/* USER CODE BEGIN 2 */
	CubeCAN_Config primaryCanConfig = {.rx_callback = CANdler_Callback, .context.busid_user_context = GRCAN_BUS_PRIMARY, .sending_node_id = GRCAN_Debugger};
	CubeCAN_Config dataCanConfig = {.rx_callback = CANdler_Callback, .context.busid_user_context = GRCAN_BUS_DATA, .sending_node_id = GRCAN_Debugger};
	CubeCAN_Config chargerCanConfig = {.rx_callback = CANdler_Callback, .context.busid_user_context = GRCAN_BUS_CHARGER, .sending_node_id = GRCAN_Debugger};

	primaryHandle = CubeCAN_Entrance(&hfdcan1, &primaryCanConfig);
	if (primaryHandle == NULL) {
		LOGOMATIC_CRITICAL("Failed to initialize primary CAN handle\n");
		Error_Handler();
	}

	dataHandle = CubeCAN_Entrance(&hfdcan2, &dataCanConfig);
	if (dataHandle == NULL) {
		LOGOMATIC_CRITICAL("Failed to initialize data CAN handle\n");
		Error_Handler();
	}

	chargerHandle = CubeCAN_Entrance(&hfdcan3, &chargerCanConfig);
	if (chargerHandle == NULL) {
		LOGOMATIC_CRITICAL("Failed to initialize charger CAN handle\n");
		Error_Handler();
	}

	LOGOMATIC_INFO("Hello World!\n");
	CubeVCP_EnableRx(CANdler_VCP_Callback);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		MainLoop();
		HAL_Delay(100);
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
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 20, LL_RCC_PLLR_DIV_2);
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
	LL_SetSystemCoreClock(160000000);

	/* Update the time base */
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/**
 * @brief Route the CubeMX-generated CAN service timer callback to the CAN runtime.
 *
 * CAN_TIMER_HANDLE is generated by can_cfg.h from CAN_TIMER_INDEX.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UNUSED(htim);
	CubeCAN_Tick();
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
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
	LOGOMATIC_CRITICAL("Assert failed! File %s on line %" PRIu32 "\n", file, line);
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
