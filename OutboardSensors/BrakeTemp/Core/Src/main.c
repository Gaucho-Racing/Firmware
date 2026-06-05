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

#include "NodeID.h"
#include "crc.h"
#include "gpio.h"
#include "i2c.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "CANdler.h"
#include "MLX90614_API.h"
#include "MLX90614_SMBus_Driver.h"
#include "brake_emissivity.h"
#include "can.h"
#include "timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// MLX stuff
static uint16_t eeMLX90614[32];
uint32_t delay = 0;
float ta = 0.0, to = 0.0;
int status;

// Wheel speed stuff
TIM_HandleTypeDef htim6;
volatile uint16_t pulses[MAX_NUM_INTERVALS];
volatile uint16_t time_deltas[MAX_NUM_INTERVALS];
volatile uint32_t total_time = 0;
volatile uint32_t total_pulses = 0;
volatile uint32_t last_tick = 0;
volatile uint32_t temp_num_pulses = 0;
volatile uint32_t temp_tick = 0;
uint32_t HAL_tick_freq_hz = 0;
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile uint8_t num_pulse_intervals = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

PUTCHAR_PROTOTYPE
{
	ITM_SendChar(ch);
	return ch;
}

// Only call this function once to flash the EEPROM of a new brake temp sensor.
// Make sure to power cycle the device afterwards.
int MLX90614_Configure_EEPROM(void)
{
	HAL_Delay(100); // For good measure
	status = MLX90614_SetEmissivity(MLX90614_ADDRESS, GR_BRAKE_EMISSIVITY);
	status = MLX90614_SetFIR(MLX90614_ADDRESS, 0x4); // 128 pt averaging
	status = MLX90614_SetIIR(MLX90614_ADDRESS, 0x4); // 100% spike limit (instant response)

	return 0;
}

// This function is triggered by TIM6 (one-pulse mode)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6) {
		// Optional: Clear any lingering noise flags on the EXTI line
		// that happened during the blanking period
		__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_11);
	}

	htim->Instance->SR &= ~TIM_SR_UIF; // clear update interrupt flag
}

// This function is triggered by the rising edge interrupt from pin B11 (wheel_speed_in)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_11) {

		// Timer-based cool-off check (Debounce)
		// If TIM6 is actively running (CEN bit is set), we are inside the cool-off window. Reject this edge.
		if ((htim6.Instance->CR1 & TIM_CR1_CEN) != 0) {
			return;
		}

		temp_tick = HAL_GetTick();
		temp_num_pulses++;

		// Reset and restart the cool-off timer peripheral
		htim6.Instance->CNT = 0;	    // reset count
		htim6.Instance->CR1 |= TIM_CR1_CEN; // restart

		// HAL_GetTick() is in ms resolution --> only push the number of pulses onto buffer
		// after at least MIN_TICK_DELTA has elapsed.
		if (temp_tick - last_tick < MIN_TICK_DELTA) {
			return;
		}

		// If buffer is full, take off the amount that is being kicked out
		if (num_pulse_intervals >= MAX_NUM_INTERVALS) {
			total_time -= time_deltas[tail];
			total_pulses -= pulses[tail];
			head = (head + 1) & (MAX_NUM_INTERVALS - 1); // Buffer overflow: advance head
		} else {
			num_pulse_intervals++;
		}

		// Push the current sample pair into the buffers
		pulses[tail] = temp_num_pulses;
		temp_num_pulses = 0;
		time_deltas[tail] = temp_tick - last_tick;

		// Increment running totals
		total_pulses += pulses[tail];
		total_time += time_deltas[tail];

		tail = (tail + 1) & (MAX_NUM_INTERVALS - 1);

		last_tick = temp_tick;
	}
}

// Note that pulses[] and time_deltas[] do NOT need to be zero'd out here
// because the head and tail indices take care of bounds.
void ResetRPMHistory(void)
{
	head = 0;
	tail = 0;
	num_pulse_intervals = 0;
	total_pulses = 0;
	total_time = 0;
	last_tick = 0;
}

float GetRPM(void)
{
	// This is more safe than just checking if total_pulses or num_pulse_intervals == 0
	if (total_time == 0) {
		return 0.0f;
	}

	float rpm = ((float)total_pulses * HAL_tick_freq_hz) / total_time; // Pulses per second
	rpm *= SECONDS_PER_MIN;						   // Pulses per min
	rpm /= PULSES_PER_ROT;						   // Rotations per min

	return rpm;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* STM32G4xx HAL library initialization:
	     - Configure the Flash prefetch
	     - Systick timer is configured by default as source of time base, but user
	       can eventually implement his proper time base source (a general purpose
	       timer for example or other time source), keeping in mind that Time base
	       duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
	       handled in milliseconds basis.
	     - Set NVIC Group Priority to 4
	     - Low Level Initialization
	   */
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CRC_Init();
	MX_I2C2_SMBUS_Init();
	/* USER CODE BEGIN 2 */

	WHEEL_SPEED_GPIO_INIT();
	WHEEL_SPEED_TIMER_INIT(&htim6, DEBOUNCE_NUM_10us);
	HAL_tick_freq_hz = 1000 / HAL_GetTickFreq();

	CANInitialize();
	can_start(can_handler);

	HAL_Delay(10);
	MLX90614_DumpEE(MLX90614_ADDRESS, eeMLX90614);
	// MLX90614_Configure_EEPROM(); // NOTE: Only call this once, power-cycle sensor, then comment out

	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	while (1) {
		if (HAL_GetTick() < delay) {
			continue; // The MLX90614 commands below take < 1 ms on average
		}

		delay = HAL_GetTick() + BRAKETEMP_INTERVAL_MS;

		NVIC_DisableIRQ(EXTI15_10_IRQn); // Enter atomic section

		// status = MLX90614_GetTa(MLX90614_ADDRESS, &ta); // Sensor ambient temperature
		status = MLX90614_GetTo(MLX90614_ADDRESS, &to); // Sensor object temperature
		CAN_sendTemp(to);

		// Reset wheel speed history after inactivity
		if (last_tick != 0 && HAL_GetTick() - last_tick > WHEEL_SPEED_TIMEOUT_TICKS) {
			ResetRPMHistory();
		}
		CAN_sendRPM(GetRPM()); // Send RPM data

		NVIC_EnableIRQ(EXTI15_10_IRQn); // Exit atomic section

		/* USER CODE END WHILE */
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
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 80;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	// Make sure that the PLL Q output is enabled (PLL R output seems to be enabled by default because its for SysClock)
	__HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL_48M1CLK);

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}

	UNUSED(Buffercmp);
}

/* USER CODE BEGIN 4 */

/**
 * @brief  Compares two buffers.
 * @param  pBuffer1, pBuffer2: buffers to be compared.
 * @param  BufferLength: buffer's length
 * @retval 0  : pBuffer1 identical to pBuffer2
 *         >0 : pBuffer1 differs from pBuffer2
 */
static uint16_t Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--) {
		if ((*pBuffer1) != *pBuffer2) {
			return BufferLength;
		}
		pBuffer1++;
		pBuffer2++;
	}

	return 0;
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
	while (1)
		;
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
