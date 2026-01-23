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
#include "dma.h"
#include "fdcan.h"
#include "gpio.h"
#include "i2c.h"
// #include "stm32g4xx_hal_ospi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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
	ITM_Enable();

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_FDCAN2_Init();
	MX_ADC1_Init();
	MX_LPUART1_UART_Init();
	MX_I2C2_Init();
	MX_USART1_UART_Init();
	// MX_SPI3_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	LOGOMATIC("Booted!\n");

	ex_config.TransferDirection = LL_SPI_FULL_DUPLEX;
	ex_config.Mode = LL_SPI_MODE_MASTER;
	ex_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	ex_config.ClockPolarity = LL_SPI_POLARITY_LOW;
	ex_config.ClockPhase = LL_SPI_PHASE_1EDGE;
	ex_config.NSS = LL_SPI_NSS_HARD_OUTPUT;
	ex_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	ex_config.BitOrder = LL_SPI_LSB_FIRST;
	ex_config.CRCCalculation = LL_SPI_CRCCALCULATION_ENABLE;
	ex_config.CRCPoly = 0x1D;

	ex_pins.SPIx = SPI1;
	ex_pins.GPIOx = (GPIO_TypeDef **)(malloc(4 * sizeof(GPIO_TypeDef *)));
	// All pins are in the A clock port
	for (int i = 0; i < 4; i++) {
		*(ex_pins.GPIOx + i) = GPIOA;
	}
	ex_pins.num_pins = 4;
	ex_pins.pin_nums = (uint32_t *)malloc(4 * sizeof(int));
	ex_pins.pin_nums[0] = LL_GPIO_PIN_7; // COPI
	ex_pins.pin_nums[1] = LL_GPIO_PIN_6; // CIPO
	ex_pins.pin_nums[2] = LL_GPIO_PIN_5; // SCK
	ex_pins.pin_nums[3] = LL_GPIO_PIN_4; // NSS
	ex_pins.alternate_function_number = 5;

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

	GR_SPI_Message msg;
	msg.data = (uint8_t*)malloc(4*sizeof(uint8_t));
	msg.size = 4;

	msg.data[0] = 'a';
	msg.data[1] = '0';
	msg.data[2] = '0';
	msg.data[3] = '0';

	GR_SPI_Send(&ex_handler, &msg);

	GR_SPI_Message* msgPtr = NULL;
	while(!msgPtr) {
		msgPtr = GR_SPI_Receive(&ex_handler);
	}

	LOGOMATIC("Received: %c\n", msgPtr->data[0]);	

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
