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

#include "GR_OLD_BUS_ID.h"
#include "StateData.h"
#include "StateTicks.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "gpio.h"
#include "gr_adc.h"
#include "malloc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CANdler.h"
#include "CANutils.h"
#include "Logomatic.h"
#include "StateTicks.h"
#include "StateUtils.h"
#include "adc.h"
#include "can.h"
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
				   .gpio_pin_rx_tx_mask = LL_GPIO_PIN_2 | LL_GPIO_PIN_3,
				   .baud_rate = 115200,
				   .data_width = LOGOMATIC_DATAWIDTH_8B,
				   .stop_bits = LOGOMATIC_STOPBITS_1,
				   .parity = LOGOMATIC_PARITY_NONE,
				   .transfer_direction = LOGOMATIC_DIRECTION_TX,
				   .hardware_flow_control = LOGOMATIC_HWCONTROL_NONE,
				   .prescaler = LOGOMATIC_PRESCALER_DIV1,
				   .tx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8,
				   .rx_fifo_threshold = LOGOMATIC_FIFOTHRESHOLD_1_8};
/* USER CODE END PV */

// CAN

#define CAN_TX_BUFFER_LENGTH 10
// ADC 1
#define WINDOW_SIZE 10 // weighted average for now can extend to other window functions
#define NUM_SIGNALS_ADC1 7
#define NUM_SIGNALS_ADC2 4
#define NUM_SIGNALS (NUM_SIGNALS_ADC1 + NUM_SIGNALS_ADC2)
#define NUM_SIGNALS_DIGITAL 8
// TODO: check which data size to use (floats...ints...etc)
volatile uint16_t ADC_buffers[NUM_SIGNALS] = {0}; // Contains new values
uint16_t ADC_outputs[NUM_SIGNALS] = {0};	  // Updated averages
uint16_t *adcDataValues[NUM_SIGNALS] = {0};	  // 2D Array

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
void read_digital(void)
{
	// TODO: implement CAN button messenge receiving

	// TODO: inertia sense? LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_10);
	stateLump.estop_sense = LL_GPIO_IsInputPinSet(ESTOP_SENSE_GPIO_Port, ESTOP_SENSE_Pin);
}

void write_adc_values_to_state_data()
{
	// analog
	// TODO: bse signal idk what to do ADC_outputs[0]
	// TODO: bspd signal --> ADC_outputs[1], find use
	stateLump.APPS1_Signal = ADC_outputs[2];
	stateLump.APPS2_Signal = ADC_outputs[3];
	stateLump.Brake_F_Signal = ADC_outputs[4];
	stateLump.Brake_R_Signal = ADC_outputs[5];
	// TODO: Aux signal idk what to do with it ADC1_outputs[6]
	stateLump.STEERING_ANGLE_SIGNAL = ADC_outputs[6];
	stateLump.bspd_sense = ADC_outputs[7];
	stateLump.imd_sense = ADC_outputs[8];
	stateLump.ams_sense = ADC_outputs[9];
}

void ADC_Configure(void)
{
	// Initialize which clock source to use
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);
	/* Peripheral clock enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	// OLD ADC (FOR REF)
	/*
	// Initialize the ADC1
	ADC_Group_Init(ADC1, PS_8); // TODO: change prescalar l8r
	ADC_Regular_Group_Init(ADC1, RANKS_7);

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
	ADC_Regular_Group_Init(ADC2, RANKS_4);

	// Initialize the pins and channels
	Pin_Ports p3 = {0};
	p3.port = GPIOA;
	p3.pin = LL_GPIO_PIN_15;
	ADC_Init_Pins(&p3);
	ADC_Channel_Init(ADC2, RANK_1, ADC_CHANNEL_15, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_2, ADC_CHANNEL_13, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_3, ADC_CHANNEL_3, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	ADC_Channel_Init(ADC2, RANK_4, ADC_CHANNEL_4, SINGLE_ENDED, SAMPLINGTIME_247CYCLES_5);
	*/

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
	SamplingTime s1 = SAMPLINGTIME_247CYCLES_5;
	Init_Vals_ADC1.SamplingTimes = &s1;
	ADC_Init(&Init_Vals_ADC1);

	// ADC 2
	ADC_Init_Values Init_Vals_ADC2 = {0};
	Init_Vals_ADC2.ADC = ADC2;
	Init_Vals_ADC2.PS_Value = PS_8;	    // TODO: change later
	Init_Vals_ADC2.Res = RESOLUTION_12; // TODO: change later
	Init_Vals_ADC2.Num_Pin_Port_Objs = 1;
	Pin_Ports p2[2] = {{.pin = BSPD_SENSE_Pin | IMD_SENSE_Pin | AMS_SENSE_Pin, .port = GPIOA}, {.pin = STEERING_ANGLE_Pin, .port = STEERING_ANGLE_GPIO_Port}};
	Init_Vals_ADC2.Pins = p2;
	Init_Vals_ADC2.Num_Channels = 4; // check multiple GPIO stuff
	Channel c2[] = {ADC_CHANNEL_15, ADC_CHANNEL_13, ADC_CHANNEL_3, ADC_CHANNEL_4};
	Init_Vals_ADC2.Channels = c2;
	SamplingTime s2 = SAMPLINGTIME_247CYCLES_5;
	Init_Vals_ADC2.SamplingTimes = &s2;
	ADC_Init(&Init_Vals_ADC2);

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
	DMA_Init_Vals_ADC1.Data_Size = Word;
	DMA_Init_Vals_ADC1.Priority = HIGH; // TODO: check what this does
	DMA_Init(&DMA_Init_Vals_ADC1);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

	DMA_Init_Values DMA_Init_Vals_ADC2 = {0};
	DMA_Init_Vals_ADC2.DMA = DMA1;
	DMA_Init_Vals_ADC2.ADC = ADC2;
	DMA_Init_Vals_ADC2.Channel = DMA_CHANNEL_2;
	DMA_Init_Vals_ADC2.Src_Address = LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA);
	DMA_Init_Vals_ADC2.Dest_Address = ADC_buffers + NUM_SIGNALS_ADC1;
	DMA_Init_Vals_ADC2.Data_Size = Word;
	DMA_Init_Vals_ADC2.Priority = HIGH; // TODO: check what this does
	DMA_Init(&DMA_Init_Vals_ADC2);
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

	ADC_Enable_And_Calibrate(ADC1);
	ADC_Enable_And_Calibrate(ADC2);
}

void CAN1_rx_callback(uint32_t ID, void *data, uint32_t size)
{
	ECU_CAN_MessageHandler(&stateLump, GR_OLD_BUS_PRIMARY,
			       (0x000FFF00 & ID) >> 8, // TODO: Double check
			       (0xFF00000 & ID) >> 20, data, size);
}

void CAN2_rx_callback(uint32_t ID, void *data, uint32_t size)
{
	ECU_CAN_MessageHandler(&stateLump, GR_OLD_BUS_DATA, (0x000FFF00 & ID) >> 8, (0xFF00000 & ID) >> 20, data, size);
}

void CAN_Configure()
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
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127; // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 8;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 15; // Updated for 170MHz: (1+15+5)*8 = 168 ticks -> ~5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 5;
	canCfg.hal_fdcan_init.StdFiltersNbr = 1;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 0;

	canCfg.rx_callback = NULL;
	canCfg.rx_interrupt_priority = 15; // TODO: Maybe make these not hardcoded
	canCfg.tx_interrupt_priority = 15;
	canCfg.tx_buffer_length = CAN_TX_BUFFER_LENGTH;

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

	primary_can = can_init(&canCfg);

	// Filter 1 Definitions
	FDCAN_FilterTypeDef fdcan1_filter;

	fdcan1_filter.IdType = FDCAN_EXTENDED_ID;
	fdcan1_filter.FilterIndex = 0;
	fdcan1_filter.FilterType = FDCAN_FILTER_MASK;
	fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	fdcan1_filter.FilterID1 = LOCAL_GR_ID; // filter messages with ECU destination
	fdcan1_filter.FilterID2 = 0x00000FF;

	fdcan1_filter.FilterIndex = 1;
	fdcan1_filter.FilterID1 = 0xFF; // filter messages for all targets
	HAL_FDCAN_ConfigFilter(primary_can->hal_fdcanP, &fdcan1_filter);

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

	// Filter definitions
	FDCAN_FilterTypeDef fdcan2_filter;

	fdcan2_filter.IdType = FDCAN_EXTENDED_ID;
	fdcan2_filter.FilterIndex = 0;
	fdcan2_filter.FilterType = FDCAN_FILTER_MASK;
	fdcan2_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // TODO: check if this works during test, RXFifos may not be indpeendent (but it sur)
	fdcan2_filter.FilterID2 = 0x00000FF;

	fdcan2_filter.FilterIndex = 1;
	fdcan2_filter.FilterID1 = 0xFF; // filter messages for all targets

	data_can = can_init(&canCfg);

	// accept unmatched standard and extended frames into RXFIFO0 - default behaviour
	HAL_FDCAN_ConfigFilter(data_can->hal_fdcanP, &fdcan2_filter);

	can_start(primary_can);
	can_start(data_can);
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
	Setup_Logomatic(&logomaticConfig);
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	// TODO: do we need these?
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_FDCAN1_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_FDCAN2_Init();
	/* USER CODE BEGIN 2 */

	// Initialize CAN
	CAN_Configure();

	ADC_Configure();
	for (int i = 0; i < NUM_SIGNALS; i++) {
		adcDataValues[i] = malloc(sizeof(uint16_t) * WINDOW_SIZE);
	}

	LOGOMATIC("Boot completed at %lu ms\n", MillisecondsSinceBoot());

	LL_GPIO_SetOutputPin(SOFTWARE_OK_CONTROL_GPIO_Port, SOFTWARE_OK_CONTROL_Pin);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		read_digital();
		// TODO: determine alpha
		ADC_UpdateAnalogValues_EMA(ADC_buffers, NUM_SIGNALS, 0.3, ADC_outputs);
		SendECUStateDataOverCAN(&stateLump);
		write_adc_values_to_state_data();
		ECU_State_Tick();
		LOGOMATIC("Main Loop Tick Complete. I use Arch btw\n");
		LL_mDelay(250); // FIXME Reduce or remove de
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
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 85, LL_RCC_PLLR_DIV_2);
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
	LOGOMATIC("Assertion failed: file %s on line %d\r\n", file, line)
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
