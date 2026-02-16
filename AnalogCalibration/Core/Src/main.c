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

#include "dma.h"
#include "gr_adc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Logomatic.h"
#include "adc.h"

// ADC 1
#define NUM_SIGNALS_ADC1 7
#define NUM_SIGNALS_ADC2 4
#define NUM_SIGNALS (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)
volatile uint16_t ADC_Buffers[NUM_SIGNALS] = {0}; // Most recent values
uint16_t ADC_Outputs[NUM_SIGNALS] = {0};	  // Updated averages

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
LogomaticConfig logomatic_config = {
    .baud_rate = 115200,
    .clock_source = LOGOMATIC_PCLK1,
    .data_width = LOGOMATIC_DATAWIDTH_8B,
    .gpio_pin_rx_tx_mask = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
    .gpio_port = LOGOMATIC_GPIOA,
    .hardware_flow_control = LOGOMATIC_HWCONTROL_NONE,
    .parity = LOGOMATIC_PARITY_NONE,
    .prescaler = LOGOMATIC_PRESCALER_DIV1,
    .stop_bits = LOGOMATIC_STOPBITS_1,
    .transfer_direction = LOGOMATIC_DIRECTION_TX,
    .tx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
    .rx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
    .bus = LOGOMATIC_BUS,
};

void ADC_Configure(void)
{
	// Initialize which clock source to use
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);
	/* Peripheral clock enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	// Initialize ADC 1
	ADC_Init_Values Init_Vals_ADC1 = {0};
	Init_Vals_ADC1.ADC = ADC1;
	Init_Vals_ADC1.PS_Value = PS_8;
	Init_Vals_ADC1.Res = RESOLUTION_12;
	Init_Vals_ADC1.Num_Pin_Port_Objs = 2;
	Pin_Ports p1[2] = {{.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3, .port = GPIOC}, {.pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_14, .port = GPIOB}};
	Init_Vals_ADC1.Pins = p1;
	Init_Vals_ADC1.Num_Channels = 7; // check multiple GPIO stuff
	Channel c1[] = {ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_15, ADC_CHANNEL_12, ADC_CHANNEL_5};
	Init_Vals_ADC1.Channels = c1;
	SamplingTime s1 = SAMPLINGTIME_247CYCLES_5;
	Init_Vals_ADC1.SamplingTimes = &s1;
	ADC_Init(&Init_Vals_ADC1);

	// ADC 2
	ADC_Init_Values Init_Vals_ADC2 = {0};
	Init_Vals_ADC2.ADC = ADC2;
	Init_Vals_ADC2.PS_Value = PS_8;
	Init_Vals_ADC2.Res = RESOLUTION_12;
	Init_Vals_ADC2.Num_Pin_Port_Objs = 1;
	Pin_Ports p2 = {LL_GPIO_PIN_15 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7, GPIOA};
	Init_Vals_ADC2.Pins = &p2;
	Init_Vals_ADC2.Num_Channels = 4; // check multiple GPIO stuff
	Channel c2[] = {ADC_CHANNEL_15, ADC_CHANNEL_13, ADC_CHANNEL_3, ADC_CHANNEL_4};
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
	DMA_Init_Vals_ADC1.Dest_Address = ADC_Buffers;
	DMA_Init_Vals_ADC1.Data_Size = Word;
	DMA_Init_Vals_ADC1.Priority = HIGH; // TODO: check what this does
	DMA_Init(&DMA_Init_Vals_ADC1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

	DMA_Init_Values DMA_Init_Vals_ADC2 = {0};
	DMA_Init_Vals_ADC2.DMA = DMA1;
	DMA_Init_Vals_ADC2.ADC = ADC2;
	DMA_Init_Vals_ADC2.Channel = DMA_CHANNEL_2;
	DMA_Init_Vals_ADC2.Src_Address = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
	DMA_Init_Vals_ADC2.Dest_Address = ADC_Buffers + NUM_SIGNALS_ADC1;
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
	Setup_Logomatic(&logomatic_config);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	ADC_Configure();

	/* USER CODE END 2 */
	LOGOMATIC("Starting Analog Calibration...\n");

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	// Array for min/max values
	uint16_t min_vals[NUM_SIGNALS];
	uint16_t max_vals[NUM_SIGNALS];
	for (int i = 0; i < NUM_SIGNALS; i++) {
		min_vals[i] = UINT16_MAX;
		max_vals[i] = 0;
	}

	char *pin_names[] = {
	    "BSE_SIGNAL", "BSPD_SIGNAL", "APPS1_SIGNAL", "APPS2_SIGNAL", "BRAKE_F_SIGNAL", "BRAKE_R_SIGNAL", "AUX_SIGNAL", "STEERING_ANGLE_SIGNAL", "BSPD_SENSE", "IMD_SENSE", "AMS_SENSE"};

	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		for (int i = 0; i < NUM_SIGNALS; i++) {
			// Update min
			if (min_vals[i] > ADC_Buffers[i]) {
				min_vals[i] = ADC_Buffers[i];
			}
			// Update max
			if (max_vals[i] < ADC_Buffers[i]) {
				max_vals[i] = ADC_Buffers[i];
			}
		}

		ADC_UpdateAnalogValues_EMA(ADC_Buffers, NUM_SIGNALS, 0.3, ADC_Outputs);
		for (int i = 0; i < NUM_SIGNALS; i++) {
			if (i == 7) {
				LOGOMATIC("%s\tMin: %d\t Max: %d\t Current Value: %d\n", pin_names[i], min_vals[i], max_vals[i], ADC_Outputs[i]);
			} else {
				LOGOMATIC("%s\t\tMin: %d\t Max: %d\t Current Value: %d\n", pin_names[i], min_vals[i], max_vals[i], ADC_Outputs[i]);
			}
		}
		LL_mDelay(250); // FIXME Reduce or remove delay
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
