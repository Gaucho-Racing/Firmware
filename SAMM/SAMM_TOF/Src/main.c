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

// #include "crc.h"
// #include "fdcan.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

#include "../Extras/VL53L4ED/VL53L4ED_api.h"
#include "IMU/bmi323.h"
// #include "circularBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BMI323_CS_GPIO_Port GPIOA // Uncommented 4/16/26 to fix reference errors
#define BMI323_CS_Pin GPIO_PIN_4  // Uncommented 4/16/26 to fix reference errors
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
PUTCHAR_PROTOTYPE
{
	ITM_SendChar(ch);
	return ch;
}
// CircularBuffer *cb;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// FDCAN_RxHeaderTypeDef RxHeader_FDCAN2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
I2C2 - is meant for thermal sensor MLX90640 and is ran using DMA to offload CPU for calculations
     - still need to fully test but ideally implmentation is done


 */
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	// cb = circular_buffer_init(64, 68 * sizeof(uint8_t));
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
	// MX_CRC_Init();
	// MX_FDCAN1_Init();
	// MX_FDCAN2_Init();
	MX_I2C1_Init();
	MX_SPI1_Init(); // TODO: change all instances of spi1 -> SPI1
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */

	// HAL_FDCAN_Start(&hfdcan1);
	// HAL_FDCAN_Start(&hfdcan2);
	// HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	// HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	bmi323 bmi323_dev;
	HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_SET);
	bmi323_init(&bmi323_dev, &hspi1, BMI323_CS_GPIO_Port, BMI323_CS_Pin); // TODO:
	// Send 2 dummy bytes to switch BMI323 to SPI mode
	// uint16_t dummy_byte = 0x8000;
	// HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_RESET);
	// HAL_SPI_Transmit(&hspi1,(uint8_t*)&dummy_byte, 1, HAL_MAX_DELAY);
	// HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_SET);
	// HAL_Delay(1);  // Short delay after mode switch

	// Initialize BMI323 sensor

	// if (BMI323_Init() != HAL_OK) {
	//   printf("BMI323 initialization failed!\r\n");
	//   Error_Handler();
	// }

	// static uint16_t eeMLX90640[832];
	// static paramsMLX90640 mlx90640;
	// #define MLX90640_ADDRESS 0x33<<1
	// MLX90640_DumpEE(MLX90640_ADDRESS, eeMLX90640);

	// MLX90640_ExtractParameters(eeMLX90640, &mlx90640);

	// MLX90640_SetRefreshRate(MLX90640_ADDRESS, 0x05);

	// MLX90640_SynchFrame(MLX90640_ADDRESS);
	//  MLX90640_SetRefreshRate(0x33, 0x05);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	// begin VL53L4ED
	HAL_Delay(100);					      // wait for 5ms to power up the device
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // TOF_L_XSHUT_Pin
	// HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET); //TOF_C_XSHUT_Pin
	HAL_Delay(100);					    // wait for 5ms to reset the device
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // TOF_L_XSHUT_Pin
	// HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET); //TOF_C_XSHUT_Pin
	HAL_Delay(100); // wait for 5ms to power up the device

	uint16_t status = 0;

	uint16_t sensor_id = 0;
	VL53L4ED_ResultsData_t results;
	uint8_t p_data_ready;

	int TOF_ID = 0x52;
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	status = VL53L4ED_GetSensorId(TOF_ID, &sensor_id);
	printf("VL53L4ED Sensor ID: 0x%04X\n", sensor_id);
	status = VL53L4ED_StartRanging(TOF_ID);
	status = VL53L4ED_SetRangeTiming(TOF_ID, 50, 70);
	status = VL53L4ED_SetOffset(TOF_ID, 50); // Set offset to 0 for testing

	while (1) {
		// HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
		// HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, TxData);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		// begin VL53L4ED
		status = VL53L4ED_CheckForDataReady(TOF_ID, &p_data_ready);
		if (p_data_ready) {
			/* (Mandatory) Clear HW interrupt to restart measurements */
			VL53L4ED_ClearInterrupt(TOF_ID);
			/* Read measured distance. RangeStatus = 0 means valid data */
			VL53L4ED_GetResult(TOF_ID, &results);
			printf("Status = %3u, Distance = %5u mm, Signal = %6u kcps/spad\n", results.range_status, results.distance_mm - 67, results.signal_per_spad_kcps);
		} else {
			HAL_Delay(10);
			__disable_irq();
			__enable_irq();
		}

		// begin BMI323
		//  int16_t ax, ay, az;
		//  if (BMI323_ReadAccel(&ax, &ay, &az) == HAL_OK) {
		//      printf("Accel: X=%d, Y=%d, Z=%d\r\n", ax, ay, az);
		//  }
		HAL_Delay(100); // Read every 100ms
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
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_RCC_HSE_EnableBypass();
	LL_RCC_HSE_Enable();
	/* Wait till HSE is ready */
	while (LL_RCC_HSE_IsReady() != 1) {}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 32, LL_RCC_PLLR_DIV_2);
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
	LL_SetSystemCoreClock(128000000);

	/* Update the time base */
	if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
// void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
// {
//   uint8_t RxData[64];
//   HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader_FDCAN2, RxData);
//   printf("got messgae\n");
//   //circularBufferPush(cb, RxData, sizeof(RxData));

// }
/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

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
