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

#include "GRCAN_BUS_ID.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_NODE_ID.h"
#include "StateData.h"
#include "StateTicks.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "gpio.h"
#include "gr_adc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "CANdler.h"
#include "CANutils.h"
#include "Lights.h"
#include "Logomatic.h"
#include "Pinging.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "adc.h"
#include "can.h"
#include "stm32g4xx_hal.h"
#include "vcp.h"
// #define PLAN_C
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
/* USER CODE END PV */

// ADC 1
#define WINDOW_SIZE 10 // weighted average for now can extend to other window functions
#define NUM_SIGNALS_ADC1 7
#define NUM_SIGNALS_ADC2 4
#define NUM_SIGNALS (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)
// TODO: check which data size to use (floats...ints...etc)
volatile uint16_t ADC_buffers[NUM_SIGNALS] = {1024}; // Contains new values
uint16_t ADC_outputs[NUM_SIGNALS] = {1024};	     // Updated averages

// DIGITAL

// STATE DATA
extern ECU_StateData stateLump;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

// TODO: state data stores stuff as either FLOATS or BOOLS...check
// TODO: TS and RTD button signals will come over CAN
void ADC_Logomatic(void)
{
	static uint16_t min_apps_1 = 4095;
	static uint16_t min_apps_2 = 4095;
	if (stateLump.APPS1_Signal < min_apps_1) {
		min_apps_1 = stateLump.APPS1_Signal;
	}
	if (stateLump.APPS2_Signal < min_apps_2) {
		min_apps_2 = stateLump.APPS2_Signal;
	}
	static uint16_t max_apps_1 = 0;
	static uint16_t max_apps_2 = 0;
	if (stateLump.APPS1_Signal > max_apps_1) {
		max_apps_1 = stateLump.APPS1_Signal;
	}
	if (stateLump.APPS2_Signal > max_apps_2) {
		max_apps_2 = stateLump.APPS2_Signal;
	}
	static uint16_t min_BSE = 4095;
	if (stateLump.bse_signal < min_BSE) {
		min_BSE = stateLump.bse_signal;
	}
	static uint16_t max_BSE = 0;
	if (stateLump.bse_signal > max_BSE) {
		max_BSE = stateLump.bse_signal;
	}

	static uint16_t min_BR = 4095;
	if (stateLump.Brake_R_Signal < min_BR) {
		min_BR = stateLump.Brake_R_Signal;
	}
	static uint16_t max_BR = 0;
	if (stateLump.Brake_R_Signal > max_BR) {
		max_BR = stateLump.Brake_R_Signal;
	}

	static uint16_t min_BF = 4095;
	if (stateLump.Brake_F_Signal < min_BF) {
		min_BF = stateLump.Brake_F_Signal;
	}
	static uint16_t max_BF = 0;
	if (stateLump.Brake_F_Signal > max_BF) {
		max_BF = stateLump.Brake_F_Signal;
	}

	static uint16_t min_BSPD = 4095;
	if (stateLump.bspd_signal < min_BSPD) {
		min_BSPD = stateLump.bspd_signal;
	}
	static uint16_t max_BSPD = 0;
	if (stateLump.bspd_signal > max_BSPD) {
		max_BSPD = stateLump.bspd_signal;
	}

	LOGOMATIC("APPS1 %d (min %d, max %d) APPS2 %d (min %d, max %d) BSE %d (min %d, max %d) BSPD %d (min %d, max %d) Brake F %d (min %d, max %d), Brake R %d (min %d, max %d), Aux %d, Steering "
		  "Angle %d\n",
		  stateLump.APPS1_Signal, min_apps_1, max_apps_1, stateLump.APPS2_Signal, min_apps_2, max_apps_2, stateLump.bse_signal, min_BSE, max_BSE, stateLump.bspd_signal, min_BSPD, max_BSPD,
		  stateLump.Brake_F_Signal, min_BF, max_BF, stateLump.Brake_R_Signal, min_BR, max_BR, stateLump.aux_signal, stateLump.steering_angle_signal);

	// static float travel1 = 0;
	// static float travel2 = 0;
	// static float travel = 0;
	// travel1 = (stateLump.APPS1_Signal - THROTTLE_MIN_1) * (1.0f - 0.0f) / (THROTTLE_MAX_1 - THROTTLE_MIN_1) + 0.0f;
	// travel2 = (stateLump.APPS2_Signal - THROTTLE_MIN_2) * (1.0f - 0.0f) / (THROTTLE_MAX_2 - THROTTLE_MIN_2) + 0.0f;
	// travel = (travel1 + travel2) / 2.0f;
	// LOGOMATIC("APPS1 Travel %f, APPS2 Travel %f, Average Travel %f\n", travel1, travel2, travel);
}

void read_digital(void)
{
	stateLump.estop_sense = LL_GPIO_IsInputPinSet(ESTOP_SENSE_GPIO_Port, ESTOP_SENSE_Pin);
}

void write_adc_values_to_state_data(void)
{
	// analog
	stateLump.bse_signal = ADC_outputs[ADC_BUFFER_SIG_BSE];
	stateLump.bspd_signal = ADC_outputs[ADC_BUFFER_SIG_BSPD];
	stateLump.APPS1_Signal = ADC_outputs[ADC_BUFFER_SIG_APPS1];
	stateLump.APPS2_Signal = ADC_outputs[ADC_BUFFER_SIG_APPS2];
	stateLump.Brake_F_Signal = ADC_outputs[ADC_BUFFER_SIG_BRAKE_F];
	stateLump.Brake_R_Signal = ADC_outputs[ADC_BUFFER_SIG_BRAKE_R];
	stateLump.aux_signal = ADC_outputs[ADC_BUFFER_SIG_AUX];
	stateLump.steering_angle_signal = ADC_outputs[ADC_BUFFER_SIG_STEERING_ANGLE]; // TODO: convert to rad/deg...?

	// TODO: determine conversion factors for all of these (uint to float)
	stateLump.bspd_sense = ADC_outputs[ADC_BUFFER_SENSE_BSPD] / 4095.0 * 3.3;
	stateLump.imd_sense = ADC_outputs[ADC_BUFFER_SENSE_IMD] / 4095.0 * 3.3;
	stateLump.bms_sense = ADC_outputs[ADC_BUFFER_SENSE_AMS] / 4095.0 * 3.3;
}

void ADC_Configure(void)
{
	// Initialize which clock source to use
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);
	/* Peripheral clock enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	// ADC 1
	ADC_Init_Values Init_Vals_ADC1 = {0};
	Init_Vals_ADC1.ADC = ADC1;
	Init_Vals_ADC1.PS_Value = PS_8;	    // TODO: change later
	Init_Vals_ADC1.Res = RESOLUTION_12; // TODO: change later
	Init_Vals_ADC1.Num_Pin_Port_Objs = 2;
	Pin_Ports p1[2] = {{.pin = BSE_SIGNAL_Pin | BSPD_SENSE_Pin | APPS1_SIGNAL_Pin | APPS2_SIGNAL_Pin, .port = GPIOC},
			   {.pin = BRAKE_F_SIGNAL_Pin | BRAKE_R_SIGNAL_Pin | AUX_SIGNAL_Pin, .port = GPIOB}};
	Init_Vals_ADC1.Pins = p1;
	Init_Vals_ADC1.Num_Channels = 7; // check multiple GPIO stuff
	Channel c1[] = {ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9, ADC_CHANNEL_15, ADC_CHANNEL_12, ADC_CHANNEL_5};
	Init_Vals_ADC1.Channels = c1;
	SamplingTime s1 = LL_ADC_SAMPLINGTIME_92CYCLES_5;
	Init_Vals_ADC1.SamplingTimes = &s1;
	ADC_Init(&Init_Vals_ADC1);

	// TODO Use ADC peripheral API for configuring ideally... but honestly its fine
	LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
	LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_256, LL_ADC_OVS_SHIFT_RIGHT_8);

	// ADC 2
	ADC_Init_Values Init_Vals_ADC2 = {0};
	Init_Vals_ADC2.ADC = ADC2;
	Init_Vals_ADC2.PS_Value = PS_8;	    // TODO: change later
	Init_Vals_ADC2.Res = RESOLUTION_12; // TODO: change later
	Init_Vals_ADC2.Num_Pin_Port_Objs = 1;
	Pin_Ports p2[2] = {{.pin = BSPD_SENSE_Pin | IMD_SENSE_Pin | AMS_SENSE_Pin, .port = GPIOA}, {.pin = STEERING_ANGLE_Pin, .port = GPIOB}};
	Init_Vals_ADC2.Pins = p2;
	Init_Vals_ADC2.Num_Channels = 4;
	Channel c2[] = {ADC_CHANNEL_13, ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_15};
	Init_Vals_ADC2.Channels = c2;
	SamplingTime s2 = LL_ADC_SAMPLINGTIME_92CYCLES_5;
	Init_Vals_ADC2.SamplingTimes = &s2;
	ADC_Init(&Init_Vals_ADC2);

	// TODO Use ADC peripheral API for configuring ideally... but honestly its fine
	LL_ADC_SetOverSamplingScope(ADC2, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
	LL_ADC_ConfigOverSamplingRatioShift(ADC2, LL_ADC_OVS_RATIO_256, LL_ADC_OVS_SHIFT_RIGHT_8);

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
	DMA_Init_Vals_ADC1.Dest_Address = ADC_buffers;
	DMA_Init_Vals_ADC1.Data_Size = Half_Word;
	DMA_Init_Vals_ADC1.Priority = HIGH; // TODO: check what this does
	DMA_Init_Vals_ADC1.Num_Data = NUM_SIGNALS_ADC1;
	DMA_Init(&DMA_Init_Vals_ADC1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

	DMA_Init_Values DMA_Init_Vals_ADC2 = {0};
	DMA_Init_Vals_ADC2.DMA = DMA1;
	DMA_Init_Vals_ADC2.ADC = ADC2;
	DMA_Init_Vals_ADC2.Channel = DMA_CHANNEL_2;
	DMA_Init_Vals_ADC2.Src_Address = LL_ADC_DMA_GetRegAddr(ADC2, LL_ADC_DMA_REG_REGULAR_DATA);
	DMA_Init_Vals_ADC2.Dest_Address = ADC_buffers + NUM_SIGNALS_ADC1;
	DMA_Init_Vals_ADC2.Data_Size = Half_Word;
	DMA_Init_Vals_ADC2.Priority = HIGH; // TODO: check what this does
	DMA_Init_Vals_ADC2.Num_Data = NUM_SIGNALS_ADC2;
	DMA_Init(&DMA_Init_Vals_ADC2);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

	ADC_Enable_And_Calibrate(ADC1);
	ADC_Enable_And_Calibrate(ADC2);
}

void CAN1_rx_callback(uint32_t ID, void *data, uint32_t size)
{
	ECU_CAN_MessageHandler(&stateLump, GRCAN_BUS_PRIMARY, (0x000FFF00 & ID) >> 8, (0xFF00000 & ID) >> 20, data, size);
}

void CAN2_rx_callback(uint32_t ID, void *data, uint32_t size)
{
	ECU_CAN_MessageHandler(&stateLump, GRCAN_BUS_DATA, (0x000FFF00 & ID) >> 8, (0xFF00000 & ID) >> 20, data, size);
}

void CAN_Configure(void)
{
	CANConfig canCfg;

	// SHARED config ddata for CAN1 and CAN2
	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_NORMAL;
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 119; // Updated for 160MHz: (1+119+40)*1 = 160 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 40;
	canCfg.hal_fdcan_init.DataPrescaler = 1;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 9; // Updated for 160MHz: 160 MHz/((1+9+10)*1) = 8 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 10;
	canCfg.hal_fdcan_init.StdFiltersNbr = 0;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 1;

	canCfg.rx_callback = NULL;
	canCfg.rx_interrupt_priority = 15; // TODO: Maybe make these not hardcoded
	canCfg.tx_interrupt_priority = 15;
	// canCfg.tx_buffer_length = CAN_TX_BUFFER_LENGTH;

	// RX shared settings
	canCfg.init_rx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_rx_gpio.Pull = GPIO_PULLUP;
	canCfg.init_rx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	// TX Shared settings
	canCfg.init_tx_gpio.Mode = GPIO_MODE_AF_PP;
	canCfg.init_tx_gpio.Pull = GPIO_NOPULL;
	canCfg.init_tx_gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	/*FDCAN_TxHeaderTypeDef TxHeader = {
	    .Identifier = 1,

	    .IdType = FDCAN_STANDARD_ID,
	    .TxFrameType = FDCAN_DATA_FRAME,
	    .ErrorStateIndicator = FDCAN_ESI_ACTIVE, // honestly this might be a value you have to read from a node
						     // FDCAN_ESI_ACTIVE is just a state that assumes there are minimal errors
	    .DataLength = 1,
	    .BitRateSwitch = FDCAN_BRS_OFF,
	    .TxEventFifoControl = FDCAN_NO_TX_EVENTS, // change to FDCAN_STORE_TX_EVENTS if you need to store info regarding transmitted messages
	    .MessageMarker = 0			      // also change this to a real address if you change fifo control
	};

	FDCANTxMessage msg = {.data = {0x80}, .tx_header = TxHeader};
	*/

	// PCLK1 from SYSCLK
	can_set_clksource(LL_RCC_FDCAN_CLKSOURCE_PCLK1);

	// CAN1 =====================================================================
	canCfg.fdcan_instance = FDCAN1;
	canCfg.rx_gpio = GPIOA;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_11;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN1;

	canCfg.tx_gpio = GPIOA;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN1;

	// RX Callback CAN1
	canCfg.rx_callback = CAN1_rx_callback; // TODO: Make sure the wrapper for this is defined correctly

	FDCAN_FilterTypeDef fdcan_primary_filter_ecu = {0};
	fdcan_primary_filter_ecu.IdType = FDCAN_EXTENDED_ID;
	fdcan_primary_filter_ecu.FilterIndex = 0;
	fdcan_primary_filter_ecu.FilterType = FDCAN_FILTER_MASK;
	fdcan_primary_filter_ecu.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan_primary_filter_ecu.FilterID1 = GRCAN_ECU & 0xFF;
	fdcan_primary_filter_ecu.FilterID2 = 0x000000FF;

	FDCAN_FilterTypeDef fdcan_primary_filter_all = {0};
	fdcan_primary_filter_all.IdType = FDCAN_EXTENDED_ID;
	fdcan_primary_filter_all.FilterIndex = 1;
	fdcan_primary_filter_all.FilterType = FDCAN_FILTER_MASK;
	fdcan_primary_filter_all.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan_primary_filter_all.FilterID1 = GRCAN_ALL & 0xFF;
	fdcan_primary_filter_all.FilterID2 = 0x000000FF;

	stateLump.primary_can = can_init(&canCfg);

	can_add_filter(stateLump.primary_can, &fdcan_primary_filter_ecu);
	can_add_filter(stateLump.primary_can, &fdcan_primary_filter_all);

	// CAN2 ======================================================
	canCfg.fdcan_instance = FDCAN2;
	canCfg.rx_gpio = GPIOB;
	canCfg.init_rx_gpio.Pin = GPIO_PIN_12;
	canCfg.init_rx_gpio.Alternate = GPIO_AF9_FDCAN2;

	canCfg.tx_gpio = GPIOB;
	canCfg.init_tx_gpio.Pin = GPIO_PIN_13;
	canCfg.init_tx_gpio.Alternate = GPIO_AF9_FDCAN2;

	// RX Callback CAN2
	canCfg.rx_callback = CAN2_rx_callback; // TODO: Make sure the wrapper for this is defined correctly

	FDCAN_FilterTypeDef fdcan_data_filter_ecu = {0};
	fdcan_data_filter_ecu.IdType = FDCAN_EXTENDED_ID;
	fdcan_data_filter_ecu.FilterIndex = 0;
	fdcan_data_filter_ecu.FilterType = FDCAN_FILTER_MASK;
	fdcan_data_filter_ecu.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan_data_filter_ecu.FilterID1 = GRCAN_ECU & 0xFF;
	fdcan_data_filter_ecu.FilterID2 = 0x000000FF;

	FDCAN_FilterTypeDef fdcan_data_filter_all = {0};
	fdcan_data_filter_all.IdType = FDCAN_EXTENDED_ID;
	fdcan_data_filter_all.FilterIndex = 1;
	fdcan_data_filter_all.FilterType = FDCAN_FILTER_MASK;
	fdcan_data_filter_all.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan_data_filter_all.FilterID1 = GRCAN_ALL & 0xFF;
	fdcan_data_filter_all.FilterID2 = 0x000000FF;

	stateLump.data_can = can_init(&canCfg);

	can_add_filter(stateLump.data_can, &fdcan_data_filter_ecu);
	can_add_filter(stateLump.data_can, &fdcan_data_filter_all);

	// timer can
	can_start(stateLump.primary_can);
	can_start(stateLump.data_can);
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

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	// TODO: Figure out GPIO Init
	MX_GPIO_Init();
	MX_DMA_Init();
	// MX_FDCAN1_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	// MX_FDCAN2_Init();
	/* USER CODE BEGIN 2 */
	Setup_Logomatic(&logomaticConfig);
	Setup_VCP(&vcp_config);

	// Initialize CAN
	CAN_Configure();

	ADC_Configure();

	LOGOMATIC("Boot completed at %lu ms\n", MillisecondsSinceBoot());

	while (MillisecondsSinceBoot() < 5000) { // Notes per Andrey and Ryan
		LL_mDelay(MAIN_LOOP_PERIOD_US / 1000);
		ADC_UpdateAnalogValues_EMA(ADC_buffers, NUM_SIGNALS, 0.01, ADC_outputs);
		write_adc_values_to_state_data();
	}

	LOGOMATIC("Initial ADC readings stabilized at %lu ms\n", MillisecondsSinceBoot());

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		// adcs
		read_digital();
		// main state lopp, queues can messages within it
		static uint32_t delay_timer;
		static uint32_t ping_timer;
		static uint32_t adc_timer;
		// ADC
		if (adc_timer % 1000 == 0) {
			static float alphas[] = {
			    0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01,
			};
			ADC_UpdateAnalogValues_EMA_Multi(ADC_buffers, NUM_SIGNALS, alphas, ADC_outputs);
			write_adc_values_to_state_data();
		}
		adc_timer++;

		if (MillisecondsSinceBoot() >= delay_timer) {
#ifdef PLAN_C
			ECU_CAN_Send(GRCAN_BUS_PRIMARY, GRCAN_Debugger, GRCAN_DEBUG_2_0, "!PLAN_C!", 8);
#endif

			delay_timer = MillisecondsSinceBoot() + (MAIN_LOOP_PERIOD_US / 1000);

			// ADC_UpdateAnalogValues_EMA(ADC_buffers, NUM_SIGNALS, 0.01, ADC_outputs);
			// void ADC_UpdateAnalogValues(uint16_t **adcDataValues, volatile uint16_t *new_values, int num_signals, int window_size, uint16_t *weighted_output)
			// ADC_UpdateAnalogValues(ADC_buffers, ADC_outputs, NUM_SIGNALS, WINDOW_SIZE, ); // TODO: figure out which alpha values to use for the different signals
			// write_adc_values_to_state_data();

			// state tick
			ECU_State_Tick();

			// preipheral updates
			SendECUStateDataOverCAN(&stateLump);
			SendECUAnalogDataOverCAN(&stateLump);
			if (MillisecondsSinceBoot() >= ping_timer) {
				ping_timer = MillisecondsSinceBoot() + (MAIN_LOOP_PERIOD_US / 500); // half period
				pingAll();
			}
			lightControl(&stateLump);

			Send_VCP_APPS(&stateLump, ADC_outputs[ADC_BUFFER_SIG_APPS1], ADC_outputs[ADC_BUFFER_SIG_APPS2]);
			// ADC_Logomatic();
		}
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
	LL_PWR_EnableRange1BoostMode();
	LL_RCC_HSI_Enable();
	/* Wait till HSI is ready */
	while (LL_RCC_HSI_IsReady() != 1) {}

	LL_RCC_HSI_SetCalibTrimming(64);
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 80, LL_RCC_PLLR_DIV_2);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();
	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1) {}

	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}

	/* Insure 1us transition state at intermediate medium speed clock*/
	for (__IO uint32_t i = (160 >> 1); i != 0; i--)
		;

	/* Set AHB prescaler*/
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_SetSystemCoreClock(160000000);

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
	LOGOMATIC("Assertion failed: file %s on line %d\r\n", file, line);
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
