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
#include "can_sus.h"
#include "gpio.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <can.h>
#include <stdio.h>

// #include "VL53L4ED_api.h"
#include "bmi323.h"
#include "mag.h"
// #include "circularBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define BMI323_CS_GPIO_Port GPIOA
#define BMI323_CS_Pin GPIO_PIN_4
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

#define BMI_ACC_ODR 0x7	   // Output data rate -> 50 Hz
#define BMI_ACC_RANGE 0x2  // +/- 8g
#define BMI_ACC_MODE 0x7   // High performance mode
#define BMI_ACC_BW 0x0	   // Sets cut off freq to ODR/2
#define BMI_ACC_AVGNUM 0x0 // No averaging

#define BMI_GYR_ODR 0x7	   // Output data rate -> 50 Hz
#define BMI_GYR_RANGE 0x4  // 2000 deg/s (default)
#define BMI_GYR_MODE 0x7   // High performance mode
#define BMI_GYR_BW 0x0	   // Sets cut off freq to ODR/2
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
static bool ewa_signed_initialization = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint16_t ewa_u(uint16_t new_value, uint16_t old_value)
{
	if (old_value == 0xFFFF) {
		return new_value;
	}
	return alpha * new_value + (1 - alpha) * old_value;
}

int16_t ewa_i(int16_t new_value, int16_t old_value)
{
	if (!ewa_signed_initialization) {
		ewa_signed_initialization = true;
		return new_value;
	}
	return alpha * new_value + (1 - alpha) * old_value;
}

uint32_t MillisecondsSinceBoot()
{
	uint32_t tick_prio = HAL_GetTickPrio();
	uint32_t tick = HAL_GetTick();
	HAL_TickFreqTypeDef freq = HAL_GetTickFreq();
	return tick * freq;
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
	MX_SPI1_Init();
	MX_SPI3_Init();
	/* USER CODE BEGIN 2 */
	SusNode_CAN_Init(GRCAN_BUS_DATA);
	SusNode_CAN_Init(GRCAN_BUS_DATA_SUBNET);
	// HAL_FDCAN_Start(&hfdcan1);
	// HAL_FDCAN_Start(&hfdcan2);
	// HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	// HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

	/*
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
	*/

	mag mag_dev = {0};
	HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);

	HAL_StatusTypeDef init_status = mag_init(&mag_dev, &hspi3, MAG_CS_GPIO_Port, MAG_CS_Pin);

	// Initialize magnetic encoder
	if (init_status != HAL_OK) {
		LOGOMATIC("MAG initialization failed!\n");
		// Error_Handler();
	}
	/* USER CODE BEGIN WHILE */

	uint16_t imu_ax = 0xFFFF;
	uint16_t imu_ay = 0xFFFF;
	uint16_t imu_az = 0xFFFF;
	uint16_t imu_gyrx = 0xFFFF;
	uint16_t imu_gyry = 0xFFFF;
	uint16_t imu_gyrz = 0xFFFF;
	uint16_t imu_temp = 0xFFFF;
	uint16_t imu_status = 0;
	uint16_t mag_temp = 0xFFFF;
	uint16_t mag_hysteresis = 0xFFFF;
	uint16_t mag_angle = 0xFFFF;
	int16_t mag_turns = 0;
	uint8_t mag_status = 0;

	uint32_t mssinceboot = MillisecondsSinceBoot();

	uint32_t last_avgcalc_ms = mssinceboot;
	uint32_t last_send_ms = mssinceboot;

	LOGOMATIC("%d", MillisecondsSinceBoot());

	IMU_Mag_Data test_data = {0};

	while (1) {
		uint32_t current_time = MillisecondsSinceBoot();

		if (current_time - last_avgcalc_ms > avgcalc_interval) {
			last_avgcalc_ms = current_time;
			/*
						imu_ax = ewa_u(bmi323_read_acc_x(&bmi323_dev), imu_ax);
						imu_ay = ewa_u(bmi323_read_acc_y(&bmi323_dev), imu_ay);
						imu_az = ewa_u(bmi323_read_acc_z(&bmi323_dev), imu_az);
						imu_gyrx = ewa_u(bmi323_read_gyr_x(&bmi323_dev), imu_gyrx);
						imu_gyry = ewa_u(bmi323_read_gyr_y(&bmi323_dev), imu_gyry);
						imu_gyrz = ewa_u(bmi323_read_gyr_z(&bmi323_dev), imu_gyrz);
						imu_temp = ewa_u(bmi323_read_temp_data(&bmi323_dev), imu_temp);
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
			*/
			mag_temp = ewa_u(mag_read_temp(&mag_dev), mag_temp);
			mag_hysteresis = ewa_u(mag_read_HANG(&mag_dev), mag_hysteresis);
			mag_angle = ewa_u(mag_read_encoder_angle(&mag_dev), mag_angle);
			mag_turns = ewa_i(mag_read_turns(&mag_dev), mag_turns);
			mag_status = check_status(&mag_dev);

			// TODO: check these conversion formulas
			int16_t mag_temp_test = mag_temp - 60;
			float mag_angle_test = mag_angle * 360.f / 4096.0f;
			float mag_hysteresis_test = mag_hysteresis * 360.f / 4096.0f;

			LOGOMATIC("Mag temperature: %d C\n", mag_temp_test);
			LOGOMATIC("Angle: %f deg\n", mag_angle_test);
			LOGOMATIC("Turns: %d\n", mag_turns);
			LOGOMATIC("Hysteresis Angle: %f\n", mag_hysteresis_test);

			// uint8_t buffer[8] = {0};
			// buffer[0] = (angle >> 8) & 0xFF;
			// buffer[1] = angle & 0xFF;

			// buffer[2] = temp;

			// buffer[3] = (turns >> 8) & 0xFF;
			// buffer[4] = turns & 0xFF;

			// // status
			// buffer[5] = bad;
			// buffer[6] = 0x00;
			// buffer[7] = 0x00;

			test_data.bmi323_acc_x = imu_ax;
			test_data.bmi323_acc_y = imu_ay;
			test_data.bmi323_acc_z = imu_az;
			test_data.bmi323_gyro_x = imu_gyrx;
			test_data.bmi323_gyro_y = imu_gyry;
			test_data.bmi323_gyro_z = imu_gyrz;
			test_data.bmi323_temp = imu_temp;
			test_data.bmi323_status = imu_status;

			test_data.mag_angle = mag_angle;
			test_data.mag_temp = mag_temp;
			test_data.mag_turns = mag_turns;
			test_data.mag_status = mag_status;
			test_data.mag_hysteresis = mag_hysteresis;
		}

		if (current_time - last_send_ms > send_interval) {
			last_send_ms = current_time;
			GRCAN_NODE_ID test_dest_node = GRCAN_TCM;
			GRCAN_MSG_ID test_msg_id = GRCAN_SUSPENSION_IMU_MAG_DATA;
			SusNode_CAN_Send(test_dest_node, test_msg_id, &test_data);

			if (imu_status != 0) {
				LOGOMATIC("IMU is cooked");
				Error_Handler();
			}

			if (mag_status != 0) {
				LOGOMATIC("Mag is cooked");
				mag_write_error(&mag_dev);
				Error_Handler();
			}
		}

		/* TODO:

			AVERAGING IS NOT POSSIBLE IN HIGH PERFORMANCE MODE -> still implement?
				static uint32_t millis_since_last = 0
				if (millis since last < b)
				only run loop every ~8-ish samples
		*/
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

	// LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1) {}

	/* Insure 1us transition state at intermediate medium speed clock*/
	for (__IO uint32_t i = (180 >> 1); i != 0; i--)
		;

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);
	LL_SetSystemCoreClock(180000000);

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
