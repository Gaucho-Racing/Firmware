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
#include "timer.h"
#include "CANdler.h"
#include "MLX90614_API.h"
#include "MLX90614_SMBus_Driver.h"
#include "can.h"
#include "emissivity.h"
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
float emissivity = GR_BRAKE_EMISSIVITY;
float ta = 0.0, to = 0.0;
int status;
uint8_t MLX90614_address = 0x5A;

// Wheel speed stuff
TIM_HandleTypeDef htim6;
volatile uint32_t pulse_time_deltas[MAX_NUM_PULSES];
uint32_t HAL_tick_freq_hz = 0;
volatile uint32_t last_tick = 0;
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile uint8_t num_pulses = 0;

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

int MLX90614_Initialize(void)
{
	HAL_Delay(100);
	uint16_t data = 65535;

	status = MLX90614_SMBusRead(MLX90614_address, 0x04, &data);
	status = MLX90614_SMBusWrite(MLX90614_address, 0x04, data-100);
	status = MLX90614_SMBusRead(MLX90614_address, 0x04, &data);
	/*
	status = MLX90614_SetEmissivity(MLX90614_address, emissivity);
	HAL_Delay(20);
	status = MLX90614_SetFIR(MLX90614_address, 4); // 128 pt averaging
	HAL_Delay(20);
	status = MLX90614_SetIIR(MLX90614_address, 4); // 100% spike limit (instant response)
	HAL_Delay(20);
	status = MLX90614_DumpEE(MLX90614_address, eeMLX90614);
	HAL_Delay(20);
	*/

	return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        // Optional: Clear any lingering noise flags on the EXTI line
        // that happened during the blanking period
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_11);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_11) {

        // Timer-based cool-off check (Debounce)
		// If TIM6 is actively running (CEN bit is set), we are inside the cool-off window. Reject this edge.
		if ((htim6.Instance->CR1 & TIM_CR1_CEN) != 0) {
			return;
		}

        uint32_t temp_tick = HAL_GetTick();

        // Enqueue the valid delta time
        pulse_time_deltas[tail] = temp_tick - last_tick;
        tail = (tail + 1) & (MAX_NUM_PULSES - 1);

        if (num_pulses < MAX_NUM_PULSES) {
            num_pulses++;
        } else {
            head = (head + 1) & (MAX_NUM_PULSES - 1); // Buffer overflow: advance head
        }

        last_tick = temp_tick;

        // Reset and restart the cool-off timer peripheral
        __HAL_TIM_SET_COUNTER(&htim6, 0);
        HAL_TIM_Base_Start_IT(&htim6);
    }
}

void ResetRPMHistory(void) {
	for(uint8_t i = 0; i < MAX_NUM_PULSES; i++) {
		pulse_time_deltas[i] = 0;
	}
	head = 0;
	tail = 0;
	num_pulses = 0;
	last_tick = 0;
}

float GetRPM(void) {
	float rpm = 0.0f;
	uint32_t time = 0;
	for(uint8_t i = 0; i < num_pulses; i++) {
		time += pulse_time_deltas[i];
	}

	if (time == 0) return 0.0f;

	rpm = ((float)num_pulses * HAL_tick_freq_hz) / time;
	rpm *= SECONDS_PER_MIN;
	rpm /= PULSES_PER_ROT;

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

	//WHEEL_SPEED_GPIO_INIT();
	//WHEEL_SPEED_TIMER_INIT(&htim6, 10);
	HAL_tick_freq_hz = 1000 / HAL_GetTickFreq();

	CANInitialize();
	can_start(can_handler);

	MLX90614_Initialize();

	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		//status = MLX90614_GetTa(MLX90614_address, &ta); // Sensor ambient temperature
		status = MLX90614_GetTo(MLX90614_address, &to); // Sensor object temperature
		if (last_tick != 0 && HAL_GetTick() - last_tick > WHEEL_SPEED_TIMEOUT_TICKS) ResetRPMHistory();

		//CAN_sendTemp(to);
		//CAN_sendRPM(GetRPM());
		HAL_Delay(BRAKETEMP_INTERVAL_MS);

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
