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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "Logomatic.h"
#include "spi.h"
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
	static GR_SPI_Handler handle_spi2;
	static LL_SPI_InitTypeDef config_spi2;
	static GR_SPI_Pins pins_spi2;
	/*HAL_OSPI_HandleTypeDef hospi;
	HAL_StatusTypeDef status;*/

	/* USER CODE END 1 */

	/* MCU
	 * Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the
	 * Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	Setup_Logomatic(&logomaticConfig);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	LOGOMATIC("Booted!\n");
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	/* USER CODE BEGIN 2 */
	LL_mDelay(1000); // Wait for peripherals to stabilize
	LOGOMATIC("Booted!\n");

	// ========== RECEIVER CONFIGURATION ==========
	config_spi2.Mode = LL_SPI_MODE_SLAVE;
	config_spi2.NSS = LL_SPI_NSS_SOFT;
	config_spi2.TransferDirection = LL_SPI_FULL_DUPLEX;
	config_spi2.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	config_spi2.ClockPolarity = LL_SPI_POLARITY_LOW;
	config_spi2.ClockPhase = LL_SPI_PHASE_1EDGE;
	config_spi2.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
	config_spi2.BitOrder = LL_SPI_MSB_FIRST;
	config_spi2.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
	config_spi2.CRCPoly = 0x1D;

	pins_spi2.SPIx = SPI2;
	pins_spi2.GPIOx = (GPIO_TypeDef **)(malloc(4 * sizeof(GPIO_TypeDef *)));
	// All pins are in the A clock port
	for (int i = 0; i < 4; i++) {
		*(pins_spi2.GPIOx + i) = GPIOB;
	}
	pins_spi2.num_pins = 4;
	// pins_spi2.pin_nums;
	pins_spi2.pin_nums[0] = LL_GPIO_PIN_15; // COPI
	pins_spi2.pin_nums[1] = LL_GPIO_PIN_14; // CIPO
	pins_spi2.pin_nums[2] = LL_GPIO_PIN_13; // SCK
	pins_spi2.pin_nums[3] = LL_GPIO_PIN_12; // NSS
	pins_spi2.alternate_function_number = 5;

	GR_SPI_Initialize(&handle_spi2, &config_spi2, &pins_spi2);

	GR_SPI_Message msg = {0};
	msg.size = 1;
	msg.data = (uint8_t *)malloc(msg.size * sizeof(uint8_t));

	for (int i = 0; i < msg.size; i++) {
		msg.data[i] = '#';
	}

	LOGOMATIC("Receiving message...\n");

	while (GR_SPI_IsRxEmpty(&handle_spi2)) {}

	GR_SPI_Receive(&handle_spi2, &msg);

	char str[33];
	memcpy(str, msg.data, msg.size * sizeof(uint8_t));
	str[msg.size] = '\0';

	LOGOMATIC("Received: %s\n", str);

	free(msg.data);
	GR_SPI_Close(&handle_spi2);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		// BLINKY
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(1000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(1000);
		LOGOMATIC("Blinking!\n");

		/* USER CODE BEGIN 3 */
	}
}

/* void test_spi_initialize(GR_SPI_Handler* handle, LL_SPI_InitTypeDef* config,
GR_SPI_Pins* pin_config){ if(GR_SPI_Initialize(&handle, &config, &pin_config)
} */

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
		// status = ERROR;
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
