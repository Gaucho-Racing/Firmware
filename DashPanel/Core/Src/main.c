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

#include "CANdler.h"
#include "dashutils.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Logomatic.h"
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
LogomaticConfig logomaticConfig = {.clock_source = LOGOMATIC_PCLK1,
				   .bus = LOGOMATIC_BUS,
				   .gpio_port = LOGOMATIC_GPIOA,
				   .gpio_pin_rx_tx_mask = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
				   .baud_rate = 115200,
				   .data_width = LOGOMATIC_DATAWIDTH_8B,
				   .stop_bits = LOGOMATIC_STOPBITS_1,
				   .parity = LOGOMATIC_PARITY_NONE,
				   .transfer_direction = LOGOMATIC_DIRECTION_TX,
				   .hardware_flow_control = LOGOMATIC_HWCONTROL_NONE,
				   .prescaler = LOGOMATIC_PRESCALER_DIV1,
				   .tx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
				   .rx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void GPIO_Interrupt_Init(void);
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
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	/* System interrupt init*/
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

	/** Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
	 */
	LL_PWR_DisableUCPDDeadBattery();

	/* USER CODE BEGIN Init */
	CANInitialize();
	GPIO_Interrupt_Init();
	Setup_Logomatic(&logomaticConfig);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	LL_mDelay(150);
	LOGOMATIC("\nBoot completed at %lu ms\n", MillisecondsSinceBoot());
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {
		/* USER CODE END WHILE */
		// LOGOMATIC("Hello from DashPanel!\n");
		// LL_mDelay(1000);
		if (canReadyToSend) {

			GR_OLD_DASH_STATUS_MSG msg_struct;

			msg_struct.led_bits = dashStatus.led_bits;
			msg_struct.ts_button = dashStatus.TSActiveButton;
			msg_struct.rtd_button = dashStatus.RTDButton;

			// Kinda weird ngl but it doesn't matter
			if (dashStatus.TSActiveButton) {
				dashStatus.TSActiveButton = 0;
			}
			if (dashStatus.RTDButton) {
				dashStatus.RTDButton = 0;
			}

			// CAN_sendPing(GR_DASH_PANEL);
			CAN_sendECU(can_handler, &msg_struct, GR_ECU);

			canReadyToSend = false;
		}

		/* USER CODE BEGIN 3 */
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
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	/**/
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);

	/**/
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);

	/**/
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);

	/**/
	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_4);

	/**/
	GPIO_InitStruct.Pin = TS_ACTIVE_BTN_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(TS_ACTIVE_BTN_GPIO_Port, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = RTD_BTN_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(RTD_BTN_GPIO_Port, &GPIO_InitStruct);

	// PA5 for the toggling LED
	GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/**/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief GPIO Interrupt Handler Setup Function
 * @param None
 * @retval None
 */
static void GPIO_Interrupt_Init(void)
{
	// Map pins to External Lines
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13); // PC13 --> EXTI 13
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE3);  // PA3 --> EXTI 3
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE4);  // PA4 --> EXTI 4

	// Initialize the Interrupts
	LL_EXTI_InitTypeDef EXTI_Init = {0};
	EXTI_Init.Line_0_31 = LL_EXTI_LINE_13; // EXTI 13
	EXTI_Init.LineCommand = ENABLE;
	EXTI_Init.Mode = LL_EXTI_MODE_IT;
	EXTI_Init.Trigger = LL_EXTI_TRIGGER_RISING;
	LL_EXTI_Init(&EXTI_Init);
	EXTI_Init.Line_0_31 = LL_EXTI_LINE_3; // EXTI 3
	LL_EXTI_Init(&EXTI_Init);
	EXTI_Init.Line_0_31 = LL_EXTI_LINE_4; // EXTI 4
	LL_EXTI_Init(&EXTI_Init);

	// Set default priority
	NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));

	// Enable Interrupt
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
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
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
