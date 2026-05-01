
 /* This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "CANdler.h"
#include "GRCAN_MSG_DATA.h"
#include "GRCAN_MSG_ID.h"
#include "GRCAN_NODE_ID.h"
#include "Logomatic.h"
#include "can.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// TODO Comment and uncomment this line as relevant
#define EXTERNAL_LOOPBACK_TEST

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef EXTERNAL_LOOPBACK_TEST
#pragma message("Testing with external loopback")
#else
#pragma message("Testing with external CAN bus")
#endif
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Enable ITM for SWO output */
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

static CANHandle *can1;

void CAN1_rx_callback(uint32_t ID, void *data, uint32_t size)
{
//#ifdef EXTERNAL_LOOPBACK_TEST
//	LoopbackTest(ID, data, size);
//#else
    CAN_MessageHandler(GRCAN_BUS_PRIMARY, (0x000FFF00 & ID) >> 8, (0xFF00000 & ID) >> 20, (uint8_t*)data, size);
//#endif
}

void sendMSG(){
		FDCANTxMessage sendECUMsg;

		sendECUMsg.tx_header.Identifier = (GRCAN_DGPS << 20) | (GRCAN_GPS_RZ << 8) | GRCAN_Debugger;
		sendECUMsg.tx_header.IdType = FDCAN_EXTENDED_ID;
		sendECUMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
		sendECUMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
		sendECUMsg.tx_header.DataLength = FDCAN_DLC_BYTES_8;
		sendECUMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
		sendECUMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
		sendECUMsg.tx_header.MessageMarker = 0;

		GRCAN_GPS_RZ_MSG message = {.theta = 0xABCD, .acc = 0x1234, .status = 0x12345678};

		memcpy(sendECUMsg.data, &message, sizeof(message));

		can_send(can1, &sendECUMsg);


	}

// CANConfig cfg1;
void CAN_Configure()
{

	CANConfig canCfg;

	// SHARED config ddata for CAN1 and CAN2
	canCfg.hal_fdcan_init.ClockDivider = FDCAN_CLOCK_DIV1;
	canCfg.hal_fdcan_init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
	canCfg.hal_fdcan_init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
#ifdef EXTERNAL_LOOPBACK_TEST
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_EXTERNAL_LOOPBACK;
#else
	canCfg.hal_fdcan_init.Mode = FDCAN_MODE_NORMAL;
#endif
	canCfg.hal_fdcan_init.AutoRetransmission = ENABLE;
	canCfg.hal_fdcan_init.TransmitPause = DISABLE;
	canCfg.hal_fdcan_init.ProtocolException = ENABLE;
	canCfg.hal_fdcan_init.NominalPrescaler = 1;
	canCfg.hal_fdcan_init.NominalSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.NominalTimeSeg1 = 127; // Updated for 170MHz: (1+127+42)*1 = 170 ticks -> 1 Mbps
	canCfg.hal_fdcan_init.NominalTimeSeg2 = 42;
	canCfg.hal_fdcan_init.DataPrescaler = 2;
	canCfg.hal_fdcan_init.DataSyncJumpWidth = 16;
	canCfg.hal_fdcan_init.DataTimeSeg1 = 12; // Updated for 170MHz: 170 MHz/((1+12+4)*2) = 5 Mbps
	canCfg.hal_fdcan_init.DataTimeSeg2 = 4;
	canCfg.hal_fdcan_init.StdFiltersNbr = 1;
	canCfg.hal_fdcan_init.ExtFiltersNbr = 0;

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

	// primary_can = can_init(&canCfg);

	// // Filter 1 Definitions
	// FDCAN_FilterTypeDef fdcan1_filter;

	// fdcan1_filter.IdType = FDCAN_EXTENDED_ID;
	// fdcan1_filter.FilterIndex = 0;
	// fdcan1_filter.FilterType = FDCAN_FILTER_MASK;
	// fdcan1_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	// fdcan1_filter.FilterID1 = LOCAL_GR_ID; // filter messages with ECU destination
	// fdcan1_filter.FilterID2 = 0x00000FF;

	// fdcan1_filter.FilterIndex = 1;
	// fdcan1_filter.FilterID1 = 0xFF; // filter messages for all targets
	// HAL_FDCAN_ConfigFilter(primary_can->hal_fdcanP, &fdcan1_filter);

	// data_can = can_init(&canCfg);

	// accept unmatched standard and extended frames into RXFIFO0 - default behaviour
	can1 = can_init(&canCfg);

	can_start(can1);
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
	Setup_Logomatic(&logomaticConfig);

	// LOGOMATIC("Logomatic initialization complete\n");
	// VCP_Send((uint8_t *)"VCP initialization complete\n", 29);
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* USER CODE BEGIN 2 */

	CAN_Configure();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		LOGOMATIC("Main loop iteration\n");
		sendMSG();

// #ifdef EXTERNAL_LOOPBACK_TEST
// 	LOGOMATIC("Sending CAN message in external loopback mode\n");
// 		FDCANTxMessage sendECUMsg;

// 		sendECUMsg.tx_header.Identifier = (0x000FFF00 & GRCAN_GPS_RZ) >> 8;
// 		sendECUMsg.tx_header.IdType = FDCAN_EXTENDED_ID;
// 		sendECUMsg.tx_header.TxFrameType = FDCAN_DATA_FRAME;
// 		sendECUMsg.tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
// 		sendECUMsg.tx_header.DataLength = FDCAN_DLC_BYTES_8;
// 		sendECUMsg.tx_header.BitRateSwitch = FDCAN_BRS_OFF;
// 		sendECUMsg.tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
// 		sendECUMsg.tx_header.MessageMarker = 0;

// 		GRCAN_GPS_RZ_MSG message = {.theta = 0xABCD, .acc = 0x1234, .status = 0x12345678};

// 		memcpy(sendECUMsg.data, &message, sizeof(message));

// 		can_send(can1, &sendECUMsg);

// #endif

		LL_mDelay(750);
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

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
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
	/* User can add his own implementation to report the file name and line
	   number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
	   file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
