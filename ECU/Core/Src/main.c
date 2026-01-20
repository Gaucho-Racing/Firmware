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

#include "StateData.h"
#include "StateTicks.h"
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
#include "StateTicks.h"
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

/*
RELAVANT PORTS AND PINS

-- ANALOG IN --
ADC 1 (ADC_1 BUFFERS array is IN ORDER from top to bottom of this list)
BSE_SIGNAL (8): PC0 -> ADC12_IN6 (ADC 1 and ADC 2)
BSPD_SIGNAL (9): PC1 -> ADC12_IN7
APPS1_SIGNAL (10): PC2 -> ADC12_IN8
APPS2_SIGNAL (11): PC3 -> ADC12_IN9
BRAKE_F_SIGNAL (24): PB0 -> ADC1_IN15
BRAKE_R_SIGNAL (25): PB1 -> ADC1_IN12
AUX_SIGNAL (36): PB14 -> ADC1_IN5

	ADC 2
STEERING_ANGLE_SIGNAL (37): PB15 -> ADC2_IN15

-- DIGITAL IN --
BSPD_SENSE (19): PA5
IMD_SENSE (20): PA6
AMS_SENSE (21): PA7
TS_ACTIVE_BTN_SENSE (54): PC12
RTD_BTN_SENSE (53): PC11
INERTIA_SW_SENSE (52): PC10
ESTOP_SENSE (51): PA15

-- DIGITAL OUT --
RTD_CONTROL (60): PB7
TSSI_R_CONTROL (59): PB6
TSSI_G_CONTROL (58): PB5
BRAKE_CONTROL (57): PB4
TS_ACTIVE_BTN_LED_CONTROL (43): PA9
RTD_BTN_LED_CONTROL (42): PA8
*/

// ADC 1/2
#define WINDOW_SIZE 10 // weighted average for now can extend to other window functions
#define NUM_SIGNALS_ADC1 7
#define NUM_SIGNALS_ADC2 1
#define NUM_SIGNALS_DIGITAL 7
// TODO: check which data size to use (floats...ints...etc)
volatile uint16_t ADC1_buffers[NUM_SIGNALS_ADC1] = {0};		      // Contains new values
volatile uint16_t ADC2_buffers[NUM_SIGNALS_ADC2] = {0};		      // Contains new values
uint16_t ADC1_outputs[NUM_SIGNALS_ADC1] = {0};			      // Updated averages
uint16_t ADC2_outputs[NUM_SIGNALS_ADC2] = {0};			      // Updated averages
uint16_t *adcDataValues[(NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)] = {0}; // 2D Array

// DIGITAL
uint8_t digital_data[NUM_SIGNALS_DIGITAL] = {0};

// STATE DATA
extern volatile ECU_StateData stateLump;

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

// TODO: state data stores stuff as either FLOATS or BOOLS...check
void read_digital(void)
{
	for (int i = 0; i < NUM_SIGNALS_DIGITAL; i++) {
		GPIO_PinState currRead;
		if (i == 0) {
			currRead = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_5);
		} else if (i == 1) {
			currRead = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6);
		} else if (i == 2) {
			currRead = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_7);
		} else if (i == 3) {
			currRead = LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_12);
		} else if (i == 4) {
			currRead = LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_11);
		} else if (i == 5) {
			currRead = LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_10);
		} else if (i == 6) {
			currRead = LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_15);
		}
		digital_data[i] = currRead;
	}
}

void write_state_data()
{
	// analog
	// TODO: bse signal idk what to do ADC1_outputs[0]
	// TODO: bspd signal idk what to do ADC1_outputs[1]
	stateLump.APPS1_Signal = ADC1_outputs[2];
	stateLump.APPS2_Signal = ADC1_outputs[3];
	stateLump.Brake_F_Signal = ADC1_outputs[4];
	stateLump.Brake_R_Signal = ADC1_outputs[5];
	// TODO: Aux signal idk what to do with it ADC1_outputs[6]
	stateLump.STEERING_ANGLE_SIGNAL = ADC2_outputs[0];

	// digital
	stateLump.bspd_sense = digital_data[0];
	stateLump.imd_sense = digital_data[1];
	stateLump.ams_sense = digital_data[2];
	stateLump.ts_active_button_engaged = digital_data[3];
	stateLump.rtd_button_engaged = digital_data[4];
	stateLump.bspd_sense = digital_data[5];
	// TODO: inertia steering wheel sense? digital_data[6]
	stateLump.estop_sense = digital_data[7];
}

void ADC_Configure(void)
{
	// Initialize which clock source to use
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);
	/* Peripheral clock enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	// Initialize the ADC1
	ADC_Group_Init(ADC1, PS_8); // TODO: change prescalar l8r
	ADC_Init(ADC1, RESOLUTION_12, RIGHT);
	ADC_Regular_Group_Init(ADC1, NO_RANKS);

	// TODO: INITIALIZE PIN_PORTS BETTER!!!
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
	ADC_Regular_Group_Init(ADC2, NO_RANKS);

	// Initialize the pins and channels
	Pin_Ports p3 = {0};
	p3.port = GPIOA;
	p3.pin = LL_GPIO_PIN_15;
	ADC_Init_Pins(&p3);
	ADC_Channel_Init(ADC2, RANK_1, ADC_CHANNEL_15, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);

	// Initialize DMA (ADC1 = CHANNEL 1, ADC2 = CHANNEL 2)
	// DMA reads into buffer
	DMA_Init(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), ADC1_buffers, LL_DMA_PDATAALIGN_HALFWORD, LL_DMA_MDATAALIGN_HALFWORD, NUM_SIGNALS_ADC1, ADC1, HIGH);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	DMA_Init(DMA1, LL_DMA_CHANNEL_2, LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA), ADC2_buffers, LL_DMA_PDATAALIGN_HALFWORD, LL_DMA_MDATAALIGN_HALFWORD, NUM_SIGNALS_ADC2, ADC2, HIGH);
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
	ADC_Configure();
	for (int i = 0; i < (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2); i++) {
		adcDataValues[i] = malloc(sizeof(uint16_t) * WINDOW_SIZE);
	}

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		read_digital();
		ADC_UpdateAnalogValues(adcDataValues, ADC1_buffers, NUM_SIGNALS_ADC1, WINDOW_SIZE, ADC1_outputs);
		ADC_UpdateAnalogValues(adcDataValues, ADC2_buffers, NUM_SIGNALS_ADC2, WINDOW_SIZE, ADC2_outputs);
		write_state_data();
		ECU_State_Tick();
		LOGOMATIC("Main Loop Tick Complete. I like Pi %f\n", 3.14159265);
		LL_mDelay(250); // FIXME Reduce or remove delay
	}
	/* USER CODE END 3 */
	for (int i = (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2) - 1; i >= 0; i--) {
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
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4) {
	}
	LL_PWR_EnableRange1BoostMode();
	LL_RCC_HSI_Enable();
	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {
	}

	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 85, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();
	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1) {
	}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
	}

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
	while (1) {
	}
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
