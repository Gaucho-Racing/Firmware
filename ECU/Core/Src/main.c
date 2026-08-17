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
#include "adc.h"
#include "dma.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "CANdler.h"
#include "CANutils.h"
#include "Lights.h"
#include "Logomatic.h"
#include "Pinging.h"
#include "Plan_C.h"
#include "StateTicks.h"
#include "StateData.h"
#include "StateUtils.h"
#include "adc.h"
#include "can.h"
#include "stm32g4xx_hal.h"
#include "vcp.h"
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
LogomaticConfig logomaticConfig = {.clock_source = LOGOMATIC_PCLK1,
				   .bus = LOGOMATIC_BUS,
				   .gpio_port = LOGOMATIC_GPIOA,
				   .gpio_pin_rx_tx_mask = LL_GPIO_PIN_9 | LL_GPIO_PIN_10,
				   .baud_rate = 115200,
				   .data_width = LOGOMATIC_DATAWIDTH_8B,
				   .stop_bits = LOGOMATIC_STOPBITS_1,
				   .parity = LOGOMATIC_PARITY_NONE,
				   .transfer_direction = LOGOMATIC_DIRECTION_TX,
				   .hardware_flow_control = LOGOMATIC_HWCONTROL_NONE,
				   .prescaler = LOGOMATIC_PRESCALER_DIV1,
				   .tx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
				   .rx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8};

VCP_Config vcp_config = {.baud_rate = 2000000,
			 .clock_source = VCP_CLOCK_PCLK,
			 .gpio_tx_rx_pin_mask = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
			 .bus_port = VCP_Port_A,
			 .parity = VCP_Parity_None,
			 .prescaler = VCP_Prescalar_Div2,
			 .stop_bits = VCP_StopBits_1,
			 .oversampling = VCP_Oversampling_16,
			 .tx_fifo_threshold = VCP_Threshold_1_8,
			 .rx_fifo_threshold = VCP_Threshold_1_8,
			 .alternate_function = LL_GPIO_AF_7,
			 .rx_callback = NULL};


#define NUM_SIGNALS_ADC1 6
#define NUM_SIGNALS_ADC2 5
#define NUM_SIGNALS (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)
volatile uint16_t ADC_buffers[NUM_SIGNALS] = {0};
volatile uint16_t ADC_outputs[NUM_SIGNALS] = {0};


// STATE DATA
extern ECU_StateData stateLump;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void write_adc_values_to_state_data(void) {
  stateLump.bse_signal = ADC_outputs[ADC_BUFFER_SIG_BSE];
  stateLump.bspd_signal = ADC_outputs[ADC_BUFFER_SIG_BSPD];
  stateLump.APPS1_Signal = ADC_outputs[ADC_BUFFER_SIG_APPS1];
  stateLump.APPS2_Signal = ADC_outputs[ADC_BUFFER_SIG_APPS2];
  stateLump.Brake_F_Signal = ADC_outputs[ADC_BUFFER_SIG_BRAKE_F];
  stateLump.Brake_R_Signal = ADC_outputs[ADC_BUFFER_SIG_BRAKE_R];
  stateLump.aux_signal = ADC_outputs[ADC_BUFFER_SIG_AUX];
  stateLump.steering_angle_signal = ADC_outputs[ADC_BUFFER_SIG_STEERING_ANGLE];

  stateLump.bspd_sense = ADC_outputs[ADC_BUFFER_SENSE_BSPD] / 4095.0f * 3.3;
  stateLump.imd_sense = ADC_outputs[ADC_BUFFER_SENSE_IMD] / 4095.0f * 3.3;
  stateLump.bms_sense = ADC_outputs[ADC_BUFFER_SENSE_BMS] / 4095.0f * 3.3;
}

void ADC_Configure(void) {
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buffers[0], NUM_SIGNALS_ADC1);
  HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&ADC_buffers[NUM_SIGNALS_ADC1], NUM_SIGNALS_ADC2);

}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	Setup_Logomatic(&logomaticConfig);
	Setup_VCP(&vcp_config);

	// Initialize CAN
	//CAN_Configure();

	ADC_Configure();
	float adc_alpha = 5000.0f / MAIN_LOOP_PERIOD_US; // around 5 time constants in one cycle of the main loop

	LOGOMATIC("Boot completed at %lu ms\n", MillisecondsSinceBoot());

	while (MillisecondsSinceBoot() < 5000) { // Notes per Andrey and Ryan
		BrakeLightControl(&stateLump);
		// GRCAN_DASH_CONFIG_MSG message = {.led_latch_flags = (!bspdFailure(stateLump) << 5) | (!imdFailure(stateLump) << 4) | (!bmsFailure(stateLump) << 3) | (bspdFailure(stateLump) << 2) |
		//					    (imdFailure(stateLump) << 1) | (bmsFailure(stateLump) << 0)};

		GRCAN_DASH_CONFIG_MSG message = {.led_latch_flags = (true << 5) | (true << 4) | (true << 3) | (false << 2) | (false << 1) | (false << 0)};

		message.led_latch_flags = ~message.led_latch_flags; // not to spec, needed as of the current iteration of the dash panel code

		ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Dash_Panel, GRCAN_DASH_CONFIG, &message, sizeof(message));

		LL_mDelay(1);
		ADC_UpdateAnalogValues_EMA(ADC_buffers, NUM_SIGNALS, adc_alpha, ADC_outputs);
		write_adc_values_to_state_data();
	}

	LOGOMATIC("Initial ADC readings stabilized at %lu ms\n", MillisecondsSinceBoot());

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
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
	LOGOMATIC("Assertion failed: file %s on line %d\r\n", file, line);
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
