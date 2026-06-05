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
#include <arm_math.h>
#include <stdio.h>

#include "CANdler.h"
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"
#include "can.h"
#include "tire_emissivity.h"
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
uint16_t MLX90640_address = 0x33;
paramsMLX90640 mlx90640;
static uint16_t eeMLX90640[832];
static uint16_t mlx90640Frame[834];
static float mlx90640To[768];
uint32_t delay = 0;
float tr = 0.0;
int status = 0;

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

int MLX90640_FullReset(void)
{
	HAL_Delay(50);
	while (MLX90640_SetRefreshRate(MLX90640_address, 0x03) != 0) {
		HAL_Delay(50);
	}
	while (MLX90640_SetInterleavedMode(MLX90640_address) != 0) {
		HAL_Delay(50);
	}
	while (MLX90640_DumpEE(MLX90640_address, eeMLX90640) < 0) {
		HAL_Delay(50);
	}
	while (MLX90640_ExtractParameters(eeMLX90640, &mlx90640) < 0) {
		HAL_Delay(50);
	}
	HAL_Delay(50);

	return 0;
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
	MX_I2C2_Init();
	/* USER CODE BEGIN 2 */

	/* Configure LED2 */
	// BSP_LED_Init(LED2);

	CANInitialize();
	can_start(can_handler);

	MLX90640_FullReset();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		if (HAL_GetTick() < delay) continue; // Note that the MLX90640 commands below takes ~129ms on average!

		delay = HAL_GetTick() + TIRETEMP_TOTAL_INTERVAL_MS;

		MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);

		tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TIRETEMP_TA_SHIFT;
		MLX90640_CalculateTo(mlx90640Frame, &mlx90640, GR_TIRE_EMISSIVITY, tr, mlx90640To);
		MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
		MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);

		MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);

		tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TIRETEMP_TA_SHIFT;
		MLX90640_CalculateTo(mlx90640Frame, &mlx90640, GR_TIRE_EMISSIVITY, tr, mlx90640To);
		MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
		MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);

		for (size_t i = 0; i < TIRETEMP_ROUNDS; i++) {
			CAN_sendTemp(mlx90640To, i);
		}

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
