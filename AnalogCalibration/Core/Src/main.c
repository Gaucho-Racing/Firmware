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

#include "GR_OLD_BUS_ID.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "gpio.h"
#include "gr_adc.h"
#include "malloc.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Logomatic.h"
#include "adc.h"
#include "can.h"

// ADC 1
#define NUM_SIGNALS_ADC1 7
#define NUM_SIGNALS_ADC2 4
#define NUM_SIGNALS (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)
volatile uint16_t ADC_buffers[NUM_SIGNALS] = {0}; // Contains new values
uint16_t ADC_outputs[NUM_SIGNALS] = {0};	  // Updated averages
uint16_t *adcDataValues[NUM_SIGNALS] = {0};	  // 2D Array

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

void ADC_Configure(void)
{
	// Initialize which clock source to use
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);
	/* Peripheral clock enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	// OLD ADC (FOR REF)
	/*
	// Initialize the ADC1
	ADC_Group_Init(ADC1, PS_8); // TODO: change prescalar l8r
	ADC_Regular_Group_Init(ADC1, RANKS_7);

	// Initialize the pins and channels
	Pin_Ports p1 = {0};
	p1.port = GPIOC;
	p1.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
	ADC_Init_Pins(&p1);
	Pin_Ports p2 = {0};
	p2.port = GPIOB;
	p2.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_14;
	ADC_Init_Pins(&p2);
	ADC_Channel_Init(ADC1, RANK_1, ADC_CHANNEL_6, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_2, ADC_CHANNEL_7, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_3, ADC_CHANNEL_8, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_4, ADC_CHANNEL_9, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_5, ADC_CHANNEL_15, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_6, ADC_CHANNEL_12, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC1, RANK_7, ADC_CHANNEL_5, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);

	// Initialize ADC2
	ADC_Init(ADC2, RESOLUTION_12, RIGHT);
	ADC_Regular_Group_Init(ADC2, RANKS_4);

	// Initialize the pins and channels
	Pin_Ports p3 = {0};
	p3.port = GPIOA;
	p3.pin = LL_GPIO_PIN_15;
	ADC_Init_Pins(&p3);
	ADC_Channel_Init(ADC2, RANK_1, ADC_CHANNEL_15, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_2, ADC_CHANNEL_13, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_3, ADC_CHANNEL_3, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_4, ADC_CHANNEL_4, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	*/

	// ADC 1
	ADC_Init_Values Init_Vals_ADC1 = {0};
	Init_Vals_ADC1.ADC = ADC1;
	Init_Vals_ADC1.PS_Value = PS_8;	    // TODO: change later
	Init_Vals_ADC1.Res = RESOLUTION_12; // TODO: change later
	Init_Vals_ADC1.Num_Pin_Port_Objs = 2;
	Pin_Ports p1[2] = {{.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3, .port = GPIOC}, {.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_14, .port = GPIOB}};
	Init_Vals_ADC1.Pins = p1;
	Init_Vals_ADC1.Num_Channels = 7; // check multiple GPIO stuff
	Channel *c1 = {ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_15, ADC_CHANNEL_12, ADC_CHANNEL_5};
	Init_Vals_ADC1.Channels = c1;
	SamplingTime s1 = SAMPLINGTIME_247CYCLES_5;
	Init_Vals_ADC1.SamplingTimes = &s1;
	ADC_Init(&Init_Vals_ADC1);

	// ADC 2
	ADC_Init_Values Init_Vals_ADC2 = {0};
	Init_Vals_ADC2.ADC = ADC2;
	Init_Vals_ADC2.PS_Value = PS_8;	    // TODO: change later
	Init_Vals_ADC2.Res = RESOLUTION_12; // TODO: change later
	Init_Vals_ADC2.Num_Pin_Port_Objs = 1;
	Pin_Ports p2 = {LL_GPIO_PIN_15 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7, GPIOA};
	Init_Vals_ADC2.Pins = &p2;
	Init_Vals_ADC2.Num_Channels = 4; // check multiple GPIO stuff
	Channel *c2 = {ADC_CHANNEL_15, ADC_CHANNEL_13, ADC_CHANNEL_3, ADC_CHANNEL_4};
	Init_Vals_ADC2.Channels = c2;
	SamplingTime s2 = SAMPLINGTIME_247CYCLES_5;
	Init_Vals_ADC2.SamplingTimes = &s2;
	ADC_Init(&Init_Vals_ADC2);

	/*
	// Initialize DMA (ADC1 = CHANNEL 1, ADC2 = CHANNEL 2)
	// DMA reads into buffer
	DMA_Init(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), ADC1_buffers, LL_DMA_PDATAALIGN_HALFWORD, LL_DMA_MDATAALIGN_HALFWORD, NUM_SIGNALS_ADC1, ADC1, HIGH);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	DMA_Init(DMA1, LL_DMA_CHANNEL_2, LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA), ADC2_buffers, LL_DMA_PDATAALIGN_HALFWORD, LL_DMA_MDATAALIGN_HALFWORD, NUM_SIGNALS_ADC2, ADC2, HIGH);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
	*/
	// Initialize DMA (for both ADCs)
	DMA_Init_Values DMA_Init_Vals_ADC1 = {0};
	DMA_Init_Vals_ADC1.DMA = DMA1;
	DMA_Init_Vals_ADC1.ADC = ADC1;
	DMA_Init_Vals_ADC1.Channel = DMA_CHANNEL_1;
	DMA_Init_Vals_ADC1.Src_Address = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
	DMA_Init_Vals_ADC1.Dest_Address = ADC_buffers;
	DMA_Init_Vals_ADC1.Data_Size = Word;
	DMA_Init_Vals_ADC1.Priority = HIGH; // TODO: check what this does
	DMA_Init(&DMA_Init_Vals_ADC1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

	DMA_Init_Values DMA_Init_Vals_ADC2 = {0};
	DMA_Init_Vals_ADC2.DMA = DMA1;
	DMA_Init_Vals_ADC2.ADC = ADC2;
	DMA_Init_Vals_ADC2.Channel = DMA_CHANNEL_2;
	DMA_Init_Vals_ADC2.Src_Address = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
	DMA_Init_Vals_ADC2.Dest_Address = ADC_buffers + NUM_SIGNALS_ADC1;
	DMA_Init_Vals_ADC2.Data_Size = Word;
	DMA_Init_Vals_ADC2.Priority = HIGH; // TODO: check what this does
	DMA_Init(&DMA_Init_Vals_ADC2);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

	ADC_Enable_And_Calibrate(ADC1);
	ADC_Enable_And_Calibrate(ADC2);
}

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
	MX_FDCAN1_Init();
	MX_FDCAN2_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_LPUART1_UART_Init();

	/* USER CODE BEGIN 2 */

	// Set Software Latch to closed
	setSoftwareLatch(1);

	// Initialize CAN
	CAN_Configure();

	ADC_Configure();
	for (int i = 0; i < NUM_SIGNALS; i++) {
		adcDataValues[i] = malloc(sizeof(uint16_t) * WINDOW_SIZE);
	}

	LOGOMATIC("Boot completed at %lu ms\n", MillisecondsSinceBoot());

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		// TODO: determine alpha
		ADC_UpdateAnalogValues_EMA(ADC_buffers, NUM_SIGNALS, 0.3, ADC_outputs);
		SendECUStateDataOverCAN(&stateLump);
		LOGOMATIC("Main Loop Tick Complete. I like Pi %f\n", 3.14159265);
		LL_mDelay(250); // FIXME Reduce or remove de
	}
	/* USER CODE END 3 */
	for (int i = NUM_SIGNALS - 1; i >= 0; i--) {
		free(adcDataValues[i]);
	}
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
