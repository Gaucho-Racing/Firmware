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
	static GR_SPI_Handler ex_handler;
	static LL_SPI_InitTypeDef ex_config;
	static GR_SPI_Pins ex_pins;
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

	ex_config.TransferDirection = LL_SPI_FULL_DUPLEX;
	ex_config.Mode = LL_SPI_MODE_MASTER;
	ex_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	ex_config.ClockPolarity = LL_SPI_POLARITY_LOW;
	ex_config.ClockPhase = LL_SPI_PHASE_1EDGE;
	ex_config.NSS = LL_SPI_NSS_SOFT;
	ex_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	ex_config.BitOrder = LL_SPI_LSB_FIRST;
	ex_config.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
	ex_config.CRCPoly = 0x1D;

	ex_pins.SPIx = SPI3;
	ex_pins.GPIOx = (GPIO_TypeDef **)(malloc(4 * sizeof(GPIO_TypeDef *)));
	// All pins are in the A clock port
	for (int i = 0; i < 3; i++) {
		*(ex_pins.GPIOx + i) = GPIOC;
	}
	ex_pins.GPIOx[3] = GPIOA;
	ex_pins.num_pins = 4;
	ex_pins.pin_nums = (uint32_t *)malloc(4 * sizeof(int));
	ex_pins.pin_nums[0] = LL_GPIO_PIN_12; // COPI
	ex_pins.pin_nums[1] = LL_GPIO_PIN_11; // CIPO
	ex_pins.pin_nums[2] = LL_GPIO_PIN_10; // SCK
	ex_pins.pin_nums[3] = LL_GPIO_PIN_4;  // NSS
	ex_pins.alternate_function_number = 6;

	GR_SPI_Initialize(&ex_handler, &ex_config, &ex_pins);

	// LOGOMATIC("-= SPI + GPIO Init Verification (Measured | Expected) =-\n");
	// /* ---------------- SPI ---------------- */
	// LOGOMATIC("TransferDirection = %lu | %lu\n", LL_SPI_GetTransferDirection(ex_pins.SPIx), ex_config.TransferDirection);
	// LOGOMATIC("Mode              = %lu | %lu\n", LL_SPI_GetMode(ex_pins.SPIx), ex_config.Mode);
	// LOGOMATIC("DataWidth         = %lu | %lu\n", LL_SPI_GetDataWidth(ex_pins.SPIx), ex_config.DataWidth);
	// LOGOMATIC("ClockPolarity     = %lu | %lu\n", LL_SPI_GetClockPolarity(ex_pins.SPIx), ex_config.ClockPolarity);
	// LOGOMATIC("ClockPhase        = %lu | %lu\n", LL_SPI_GetClockPhase(ex_pins.SPIx), ex_config.ClockPhase);
	// LOGOMATIC("NSS               = %lu | %lu\n", LL_SPI_GetNSSMode(ex_pins.SPIx), ex_config.NSS);
	// LOGOMATIC("BaudRate          = %lu | %lu\n", LL_SPI_GetBaudRatePrescaler(ex_pins.SPIx), ex_config.BaudRate);
	// LOGOMATIC("BitOrder          = %lu | %lu\n", LL_SPI_GetTransferBitOrder(ex_pins.SPIx), ex_config.BitOrder);
	// LOGOMATIC("CRC Enable        = %lu | 1\n", LL_SPI_IsEnabledCRC(ex_pins.SPIx));
	// LOGOMATIC("CRC Polynomial    = 0x%lx | 0x%lx\n", ex_pins.SPIx->CRCPR, ex_config.CRCPoly);
	// LOGOMATIC("SPI Enable        = %lu | 1\n", LL_SPI_IsEnabled(ex_pins.SPIx));

	// uint32_t spi_clk_en = 0;
	// if (ex_handler.pins->SPIx == SPI1) {
	// 	spi_clk_en = LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_SPI1);
	// } else if (ex_handler.pins->SPIx == SPI2) {
	// 	spi_clk_en = LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI2);
	// } else if (ex_handler.pins->SPIx == SPI3) {
	// 	spi_clk_en = LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_SPI3);
	// }
	// LOGOMATIC("SPI Clock Enable  = %lu | 1\n", spi_clk_en);
	// /* ---------------- GPIO CLOCKS ---------------- */
	// for (int i = 0; i < ex_pins.num_pins; i++) {
	// 	uint32_t clk_en = 0;

	// 	if (ex_pins.GPIOx[i] == GPIOA) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	// 	} else if (ex_pins.GPIOx[i] == GPIOB) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	// 	} else if (ex_pins.GPIOx[i] == GPIOC) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	// 	} else if (ex_pins.GPIOx[i] == GPIOD) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOD);
	// 	} else if (ex_pins.GPIOx[i] == GPIOE) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOE);
	// 	} else if (ex_pins.GPIOx[i] == GPIOF) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOF);
	// 	} else if (ex_pins.GPIOx[i] == GPIOG) {
	// 		clk_en = LL_AHB2_GRP1_IsEnabledClock(LL_AHB2_GRP1_PERIPH_GPIOG);
	// 	}

	// 	LOGOMATIC("GPIO Clock [%p] = %lu | 1\n", (void *)ex_pins.GPIOx[i], clk_en);
	// }
	// /* ---------------- GPIO MODE + AF ---------------- */
	// for (int i = 0; i < ex_pins.num_pins; i++) {
	// 	uint32_t pin = ex_pins.pin_nums[i];

	// 	LOGOMATIC("GPIO[%d] Mode     = %lu | %lu\n", pin, LL_GPIO_GetPinMode(ex_pins.GPIOx[i], pin), LL_GPIO_MODE_ALTERNATE);

	// 	LOGOMATIC("GPIO[%d] AF       = %lu | %lu\n", pin, (ex_pins.pin_nums[i] < LL_GPIO_PIN_8) ? LL_GPIO_GetAFPin_0_7(ex_pins.GPIOx[i], pin) : LL_GPIO_GetAFPin_8_15(ex_pins.GPIOx[i], pin),
	// 		  ex_pins.alternate_function_number);
	// }
	// LOGOMATIC("-= End Verification =-\n");

	LOGOMATIC("Starting message transaction...\n");
	/*
	uint8_t data = 0x00;
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4); // NSS low

	for(uint8_t i = 1; i < 128; i++) {
		LL_SPI_TransmitData8(SPI3, i);
		while (LL_SPI_IsActiveFlag_BSY(SPI3));
		data = LL_SPI_ReceiveData8(SPI3);
		LOGOMATIC("data: %d\n", data);
	}

	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);   // NSS high
	*/
	GR_SPI_Message msg;
	msg.data = (uint8_t *)malloc(32 * sizeof(uint8_t));
	msg.size = 32;

	for (int i = 0; i < msg.size; i++) {
		msg.data[i] = 'A' + i;
	}

	GR_SPI_Send(&ex_handler, &msg);

	LOGOMATIC("Sent message, now receiving...\n");

	for (int i = 0; i < msg.size; i++) {
		msg.data[i] = '#';
	}

	while (GR_SPI_IsRxEmpty(&ex_handler)) {}

	GR_SPI_Receive(&ex_handler, &msg);

	char str[33];
	memcpy(str, msg.data, msg.size);
	str[32] = '\0';

	LOGOMATIC("Received: %s\n", str);

	free(msg.data);
	GR_SPI_Close(&ex_handler);
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
