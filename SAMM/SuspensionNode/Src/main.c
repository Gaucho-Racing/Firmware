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
#include "NodeID.h"
#include "gpio.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <can.h>
#include <stdio.h>

// #include "VL53L4ED_api.h"
#include "mag.h"
// #include "circularBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MAG_CS_GPIO_Port GPIOB
#define MAG_CS_Pin GPIO_PIN_10
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
	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	CANConfig my_cfg;

	get_cfg(FDCAN1, on_receive, &my_cfg, FDCAN_MODE_NORMAL);

	CANHandle *h1 = can_init(&my_cfg);

	can_start(h1);
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
	MX_SPI1_Init();
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */

	// HAL_FDCAN_Start(&hfdcan1);
	// HAL_FDCAN_Start(&hfdcan2);
	// HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	// HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	// bmi323 bmi323_dev;
	mag mag_dev;
	HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);

	// Send 2 dummy bytes to switch BMI323 to SPI mode
	// uint16_t dummy_byte = 0x8000;
	// HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_RESET);
	// HAL_SPI_Transmit(&hspi1,(uint8_t*)&dummy_byte, 1, HAL_MAX_DELAY);
	// HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_SET);
	// HAL_Delay(1);  // Short delay after mode switch

	if (mag_init(&mag_dev, &hspi3, MAG_CS_GPIO_Port, MAG_CS_Pin) != HAL_OK) {
		printf("MAG initialization failed!\r\n");
		Error_Handler();
	}

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
	// HAL_Delay(100);					      // wait for 5ms to power up the device
	// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // TOF_L_XSHUT_Pin
	// // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET); //TOF_C_XSHUT_Pin
	// HAL_Delay(100);					    // wait for 5ms to reset the device
	// HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // TOF_L_XSHUT_Pin
	// // HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET); //TOF_C_XSHUT_Pin
	// HAL_Delay(100); // wait for 5ms to power up the device

	// uint16_t status = 0;

	// uint16_t sensor_id = 0;
	// VL53L4ED_ResultsData_t results;
	// uint8_t p_data_ready;

	// int TOF_ID = 0x52;
	// HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
	// status = VL53L4ED_GetSensorId(TOF_ID, &sensor_id);
	// printf("VL53L4ED Sensor ID: 0x%04X\n", sensor_id);
	// status = VL53L4ED_StartRanging(TOF_ID);
	// status = VL53L4ED_SetRangeTiming(TOF_ID, 50, 70);
	// status = VL53L4ED_SetOffset(TOF_ID, 50); // Set offset to 0 for testing

	while (1) {
		// HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData);
		// HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, TxData);
		/* USER CODE END WHILE */

		uint8_t temp = mag_read_temp(&mag_dev);
		uint16_t angle = mag_read_encoder_angle(&mag_dev);
		int16_t turns = mag_read_turns(&mag_dev);
		// float hang = mag_read_HANG(mag_dev);
		FDCANTxMessage temp_can = {.tx_header = 0, .data[8] = temp};
		FDCANTxMessage angle_can = {.tx_header = 0, .data[16] = temp};
		FDCANTxMessage turns_can = {.tx_header = 0, .data[16] = temp};
		bool bad = check_status(&mag_dev);
		printf("Temperature is %d\n", temp);
		printf("Angle is %d\n", angle);
		printf("Number of turns is %d\n", turns);
		if (bad) {
			printf("something is cooked");
			mag_write_error(&mag_dev);
		}
		can_send(h1, &temp_can);
		can_send(h1, &angle_can);
		can_send(h1, &turns_can);
		HAL_Delay(10);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
	while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {}

	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE3);
	while (LL_PWR_IsActiveFlag_VOS() == 0) {}
	LL_RCC_HSI_Enable();

	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {}

	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_HSI_SetDivider(LL_RCC_HSI_DIV_2);
	LL_RCC_CSI_Enable();

	/* Wait till CSI is ready */
	while (LL_RCC_CSI_IsReady() != 1) {}

	LL_RCC_CSI_SetCalibTrimming(32);
	LL_RCC_PLL1_SetSource(LL_RCC_PLL1SOURCE_CSI);
	LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
	LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
	LL_RCC_PLL1_SetM(1);
	LL_RCC_PLL1_SetN(32);
	LL_RCC_PLL1_SetP(2);
	LL_RCC_PLL1_SetQ(2);
	LL_RCC_PLL1_SetR(2);
	LL_RCC_PLL1Q_Enable();
	LL_RCC_PLL1_Enable();

	/* Wait till PLL is ready */
	while (LL_RCC_PLL1_IsReady() != 1) {}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {}

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);
	LL_SetSystemCoreClock(32000000);

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
