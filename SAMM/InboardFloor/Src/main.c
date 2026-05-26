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
#include "can_inboardfloor.h"
#include "gpio.h"
#include "i2c.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <can.h>
#include <stdio.h>

#include "VL53L4ED_api.h"
#include "bmi323.h"
// #include "circularBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BMI323_CS_GPIO_Port GPIOA
#define BMI323_CS_Pin GPIO_PIN_4

#define TOF_XSHUT_GPIO_Port GPIOA
#define TOF_XSHUT_Pin GPIO_PIN_1
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

#define BMI_ACC_ODR 0x7 // Output data rate -> 50 Hz
#define BMI_ACC_RANGE 0x2 // +/- 8g
#define BMI_ACC_MODE 0x7 // High performance mode
#define BMI_ACC_BW 0x0 // Sets cut off freq to ODR/2
#define BMI_ACC_AVGNUM 0x0 // No averaging

#define BMI_GYR_ODR 0x7 // Output data rate -> 50 Hz
#define BMI_GYR_RANGE 0x4 // 2000 deg/s (default)
#define BMI_GYR_MODE 0x7 // High performance mode
#define BMI_GYR_BW 0x0 // Sets cut off freq to ODR/2
#define BMI_GYR_AVGNUM 0x0 // No averaging
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
const uint16_t avgcalc_interval = 100;
const uint16_t send_interval = 100;
const float alpha = 0.2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// TODO: MOVE THIS FUNCTION ELSEWHERE
uint16_t ewa(uint16_t new_value, uint16_t old_value) {
	if (old_value == 0xFFFF) {
		return new_value;
	}
	return alpha * new_value + (1 - alpha) * old_value;
}

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
	MX_SPI1_Init();

	/* USER CODE BEGIN 2 */
	/* USER CODE BEGIN Init */
	InboardFloor_CAN_Init(GRCAN_BUS_DATA);
	InboardFloor_CAN_Init(GRCAN_BUS_DATA_SUBNET);

	// HAL_FDCAN_Start(&hfdcan1);
	// HAL_FDCAN_Start(&hfdcan2);
	// HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	// HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

	bmi323 bmi323_dev;
	HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_SET);

	// Initialize IMU
	if (bmi323_init(&bmi323_dev, &hspi1, BMI323_CS_GPIO_Port, BMI323_CS_Pin) != HAL_OK) {
		LOGOMATIC("BMI323 initialization failed!\n");
		Error_Handler();
	}

	bmi323_soft_reset(&bmi323_dev);
	HAL_Delay(10);

	// Switch to IMU to SPI mode
	uint8_t bmi323_mode_tx[4] = {0x00 | 0x80, 0x00, 0x00, 0x00};
	HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, bmi323_mode_tx, 4, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(BMI323_CS_GPIO_Port, BMI323_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(1); // Short delay after mode switch

	// Set up accelerometer and gyroscope configuration
	bmi323_enable_acc(&bmi323_dev, BMI_ACC_MODE, BMI_ACC_AVGNUM, BMI_ACC_BW, BMI_ACC_RANGE, BMI_ACC_ODR);
	bmi323_enable_gyro(&bmi323_dev, BMI_GYR_MODE, BMI_GYR_AVGNUM, BMI_GYR_BW, BMI_GYR_RANGE, BMI_GYR_ODR);

	/* Infinite loop */
	// begin VL53L4ED
	HAL_Delay(100);					      // wait for 5ms to power up the device
	HAL_GPIO_WritePin(TOF_XSHUT_GPIO_Port, TOF_XSHUT_Pin, GPIO_PIN_RESET); // TOF_L_XSHUT_Pin
	// HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_RESET); //TOF_C_XSHUT_Pin
	HAL_Delay(100);					    // wait for 5ms to reset the device
	HAL_GPIO_WritePin(TOF_XSHUT_GPIO_Port, TOF_XSHUT_Pin, GPIO_PIN_SET); // TOF_L_XSHUT_Pin
	// HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1, GPIO_PIN_SET); //TOF_C_XSHUT_Pin
	HAL_Delay(100); // wait for 5ms to power up the device

	uint16_t status = 0;
	uint16_t sensor_id = 0;
	VL53L4ED_ResultsData_t results;
	uint8_t p_data_ready;

	int TOF_ID = 0x52;
	HAL_GPIO_TogglePin(TOF_XSHUT_GPIO_Port, TOF_XSHUT_Pin);
	status = VL53L4ED_GetSensorId(TOF_ID, &sensor_id);
	LOGOMATIC("VL53L4ED Sensor ID: 0x%04X\n", sensor_id);

	status = VL53L4ED_StartRanging(TOF_ID);
	status = VL53L4ED_SetRangeTiming(TOF_ID, 50, 70);
	status = VL53L4ED_SetOffset(TOF_ID, 0); // Set offset to 0 for testing, 50 otherwise?

	uint16_t imu_ax = 0xFFFF;
	uint16_t imu_ay = 0xFFFF;
	uint16_t imu_az = 0xFFFF;
	uint16_t imu_gyrx = 0xFFFF;
	uint16_t imu_gyry = 0xFFFF;
	uint16_t imu_gyrz = 0xFFFF;
	uint16_t imu_temp = 0xFFFF;
	uint16_t imu_status = 0;

	results.distance_mm = 0xFFFF;
	results.ambient_rate_kcps = 0xFFFF;
	results.ambient_per_spad_kcps = 0xFFFF;
	results.signal_rate_kcps = 0xFFFF;
	results.signal_per_spad_kcps = 0xFFFF;
	results.number_of_spad = 0xFFFF;
	results.sigma_mm = 0xFFFF;
	results.range_status = 0xFFFF;

	uint32_t last_avgcalc_ms = MillisecondsSinceBoot();
	uint32_t last_send_ms = MillisecondsSinceBoot();
	IMU_ToF_Data test_data = {0};
	/* USER CODE END 2 */

	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE BEGIN 3 */
		uint32_t current_time = MillisecondsSinceBoot();

		if (current_time - last_avgcalc_ms > avgcalc_interval) {
			last_avgcalc_ms = current_time;

			imu_ax = ewa(bmi323_read_acc_x(&bmi323_dev), imu_ax);
			imu_ay = ewa(bmi323_read_acc_y(&bmi323_dev), imu_ay);
			imu_az = ewa(bmi323_read_acc_z(&bmi323_dev), imu_az);
			imu_gyrx = ewa(bmi323_read_gyr_x(&bmi323_dev), imu_gyrx);
			imu_gyry = ewa(bmi323_read_gyr_y(&bmi323_dev), imu_gyry);
			imu_gyrz = ewa(bmi323_read_gyr_z(&bmi323_dev), imu_gyrz);
			imu_temp = ewa(bmi323_read_temp_data(&bmi323_dev), imu_temp);
			imu_status = bmi323_read_status(&bmi323_dev);

			float imu_ax_test = ((float)imu_ax) / 4096.f;
			float imu_ay_test = ((float)imu_ay) / 4096.f;
			float imu_az_test = ((float)imu_az) / 4096.f;
			float imu_gyrx_test = ((float)imu_gyrx) / 16.384f;
			float imu_gyry_test = ((float)imu_gyry) / 16.384f;
			float imu_gyrz_test = ((float)imu_gyrz) / 16.384f;
			float imu_temp_test = ((float)imu_temp / 512.f) + 23.0f;

			LOGOMATIC("Acceleration: x = %f g, y = %f g, z = %f g\n", imu_ax_test, imu_ay_test, imu_az_test);
			LOGOMATIC("Angular rate: x = %f deg/s, y = %f deg/s, z = %f deg/s\n", imu_gyrx_test, imu_gyry_test, imu_gyrz_test);
			LOGOMATIC("IMU temperature: %f", imu_temp_test);

			// begin VL53L4ED
			status = VL53L4ED_CheckForDataReady(TOF_ID, &p_data_ready);
			if (p_data_ready) {
				/* (Mandatory) Clear HW interrupt to restart measurements */
				VL53L4ED_ClearInterrupt(TOF_ID);

				VL53L4ED_ResultsData_t old_results = results;

				/* Read measured distance. RangeStatus = 0 means valid data */
				VL53L4ED_GetResult(TOF_ID, &results);
				results.distance_mm = ewa(results.distance_mm, old_results.distance_mm);
				results.ambient_rate_kcps = ewa(results.ambient_rate_kcps, old_results.ambient_rate_kcps);
				results.ambient_per_spad_kcps = ewa(results.ambient_per_spad_kcps, old_results.ambient_per_spad_kcps);
				results.signal_rate_kcps = ewa(results.signal_rate_kcps, old_results.signal_rate_kcps);
				results.signal_per_spad_kcps = ewa(results.signal_per_spad_kcps, old_results.signal_per_spad_kcps);
				results.number_of_spad = ewa(results.number_of_spad, old_results.number_of_spad);
				results.sigma_mm = ewa(results.sigma_mm, old_results.sigma_mm);

				LOGOMATIC("Status = %3u & Internal = %3u, Distance = %5u mm, Signal = %6u kcps/spad\n", results.range_status, status, results.distance_mm - 67, results.signal_per_spad_kcps);
			} else {
				HAL_Delay(10);
				__disable_irq();
				__enable_irq();
			}


			test_data.bmi323_acc_x = imu_ax;
			test_data.bmi323_acc_y = imu_ay;
			test_data.bmi323_acc_z = imu_az;
			test_data.bmi323_gyro_x = imu_gyrx;
			test_data.bmi323_gyro_y = imu_gyry;
			test_data.bmi323_gyro_z = imu_gyrz;
			test_data.bmi323_temp = imu_temp;
			test_data.bmi323_status = imu_status;

			test_data.distance_mm = results.distance_mm;
			test_data.ambient_rate_kcps = results.ambient_rate_kcps;
			test_data.ambient_per_spad_kcps = results.ambient_per_spad_kcps;
			test_data.signal_rate_kcps = results.signal_rate_kcps;
			test_data.signal_per_spad_kcps = results.signal_per_spad_kcps;
			test_data.number_of_spad = results.number_of_spad;
			test_data.sigma_mm = results.sigma_mm;
			test_data.range_status = results.range_status;

		}

		if (current_time - last_send_ms > send_interval) {
			last_send_ms = current_time;

			GRCAN_NODE_ID dest_node = GRCAN_TCM;
			GRCAN_MSG_ID msg_id = GRCAN_INBOARDFLOOR_IMU_TOF_DATA;
			InboardFloor_CAN_Send(dest_node, msg_id, &test_data);

			if (imu_status != 0) {
				LOGOMATIC("IMU is cooked");
				Error_Handler();
			}
			if (results.range_status != 0) {
				LOGOMATIC("ToF is cooked");
				Error_Handler();
			}
		}

	/* USER CODE END 3 */
	}
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
