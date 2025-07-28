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
#include "fdcan.h"
#include "usart.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "acu.h" // fetch bcc & other componentes here
#include "mcu.h" // might not need
#include "state.h" // fetch states
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_SIZE 6U
#define TPL_RX_BUFFER 256U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// trackers
uint8_t cycle = 0;
uint32_t prev = 0;
bool first_init = true;
char print_buffer[1000];
bool check_ts_active = false;
uint8_t bcc_cooked_count = 0;
bcc_status_t bcc_error = BCC_STATUS_SUCCESS; 
uint16_t cps = 0; // cycles per second

// BMS/ACU
ACU acu;
Battery battery;

// ADC Data
uint16_t adc_data[ADC_SIZE]; // 0: ts_current, 1: ts_voltage, 2: sdc_volt_w, 3: sdc_volt_v, 4:volt_12v, 5: water_sense

// BCC - timeout vars
uint32_t BCC_MCU_Timeout_Start = 0;
uint32_t BCC_MCU_Timeout_length = 0;

// communication BCC - TPL
volatile uint8_t TPL_RxBuffer[TPL_RX_BUFFER]; // store rx SPI data
volatile uint8_t TPL_RxBufferLevel = 0; // bytes to be read
volatile uint8_t TPL_RxBufferBottom = 0; // idx to oldest data
volatile uint8_t TPL_RxBufferTop = 0; // idx to newest data

// communication - FDCAN
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;

// communication headers - FDCAN
FDCAN_TxHeaderTypeDef TxHeader; // PRIMARY
FDCAN_RxHeaderTypeDef RxHeader; // PRIMARY

FDCAN_TxHeaderTypeDef TxHeader_Data;
FDCAN_RxHeaderTypeDef RxHeader_Data;

FDCAN_TxHeaderTypeDef TxHeader_Charger;
FDCAN_RxHeaderTypeDef RxHeader_Charger;

// communication queues - FDCAN (ensure mods are atomic)
volatile uint8_t prim_q[256] = {0}, data_q[256] = {0}, charger_q[256] = {0}; 
volatile uint8_t p_top = 0, p_bottom = 0, p_level = 0;
volatile uint8_t d_top = 0, d_bottom = 0, d_level = 0;
volatile uint8_t c_top = 0, c_bottom = 0, c_level = 0;

// communication flags - FDCAN (ensure mods are atomic)
volatile uint8_t CAN_1_flag = 0;
volatile uint8_t CAN_2_flag = 0;
volatile uint8_t CAN_3_flag = 0;

// SHARED BUFFER: all data enter in through this buffer (ensure mods are atomic)
uint8_t CAN_TxData[64] = {0};
uint8_t CAN_RxData[64] = {0};

// SHARED BUFFER: ACCESSIBLE BY INTERRUPT HANDLER AND DEV (ME!) (ensure mods are atomic)
volatile CAN_RX_message CAN_RxBuffer[256]; // Array to store received CAN data
volatile uint8_t CAN_RxBufferLevel = 0; // tells you level = abs(top - bottom)
volatile uint8_t CAN_RxBufferBottom = 0; // Index of oldest data
volatile uint8_t CAN_RxBufferTop = 0; // Index of newest data

// STATE tracker
State state;

extern void print_bcc_status(bcc_status_t stat);
extern void update_adc_data(ACU* acu);
extern void dequeue(ACU* acu);
extern void debug();

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void dequeue_in_irq();
void parse_data();

void DWT_Delay_Init();
void print_lpuart(char* arr);
int16_t Read_ADC1_Channel(uint32_t channel);

// interrupt
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/// @brief lazy way to read without the need for acu pointer to be passed
void parse_data(){
  can_read_handler(&acu);
}

/// @brief lazy way to send data without the need for acu pointer to be passed
void dequeue_in_irq(){
  dequeue(&acu);
}

/// @brief out "print" function
/// @param arr 
void print_lpuart(char* arr) {
  uint32_t idx = 0; // index
  while (arr[idx]) {
    while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
    LL_LPUART_TransmitData8(LPUART1, arr[idx]);
    idx++;
  }
}

/// @brief setup function => simplified
/// @return 0 for success, 1 for FAILURE 🤦‍♂🥲
int setup(){

  if (LL_ADC_IsEnabled(ADC1) == 0){
    // setup ADC
    LL_ADC_DisableDeepPowerDown(ADC1);
    LL_ADC_EnableInternalRegulator(ADC1);
    uint32_t wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while(wait_loop_index != 0)
    {
      wait_loop_index--;
    }
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
      ;
    }
    LL_ADC_Enable(ADC1);
    while (!LL_ADC_IsActiveFlag_ADRDY(ADC1));
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)&ADC1->DR);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)adc_data);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 6);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_ADC_REG_StartConversion(ADC1);
  }
  
  state = INIT;
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
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_TIM5_Init();
  MX_LPUART1_UART_Init();
  MX_FDCAN1_Init();
  MX_ADC1_Init();
  MX_FDCAN2_Init();
  MX_FDCAN3_Init();
  MX_TIM7_Init();
  
  /* USER CODE BEGIN 2 */
  DWT_Delay_Init();
  write_IRneg(false);
  write_IRpos(false);
  write_prechg(false);
  // write_LED(1);
  // HAL_Delay(100);
  // write_LED(0);



  /* Enable the SPI peripherals */
  BCC_MCU_WriteCsbPin(0, 1);
  LL_SPI_Enable(SPI1);
  LL_SPI_Enable(SPI2);
  LL_SPI_EnableIT_RXNE(SPI2);

  /* Enable the CAN module */
  if (  (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
      || (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
      || ( HAL_FDCAN_Start(&hfdcan3) != HAL_OK)) {
    print_lpuart("failed to HAL_FDCAN_Start\n");
  }

  // Activate interrupting capabilities
  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_FULL, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_FULL, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_FULL, 0);

  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0);
  HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, 0);

  // enable microsecond timer
  LL_TIM_EnableCounter(TIM5);
  // enable CAN handler timer interrupt
  LL_TIM_EnableCounter(TIM7);
  LL_TIM_EnableIT_UPDATE(TIM7);

  write_LED(1);
  setup();
  
  // Configure TxHeader
  TxHeader.IdType = FDCAN_EXTENDED_ID;
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; 

  TxHeader_Data.IdType = FDCAN_EXTENDED_ID;
  TxHeader_Data.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader_Data.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader_Data.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader_Data.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader_Data.TxEventFifoControl = FDCAN_NO_TX_EVENTS; 

  TxHeader_Charger.IdType = FDCAN_EXTENDED_ID;
  TxHeader_Charger.TxFrameType = FDCAN_DATA_FRAME;
  TxHeader_Charger.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  TxHeader_Charger.BitRateSwitch = FDCAN_BRS_OFF;
  TxHeader_Charger.FDFormat = FDCAN_CLASSIC_CAN;
  TxHeader_Charger.TxEventFifoControl = FDCAN_NO_TX_EVENTS; 
  
  // Configure RxHeader
  RxHeader.IdType = FDCAN_EXTENDED_ID;
  RxHeader.RxFrameType = FDCAN_DATA_FRAME;
  RxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  RxHeader.DataLength = FDCAN_DLC_BYTES_8;
  RxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  RxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  RxHeader.RxTimestamp = 0; /* Specifies the timestamp counter value captured on start of frame reception. Between 0 and 0xFFFF  */           

  // configure driveConfig
  battery.drvConfig.commMode = BCC_MODE_TPL;
  battery.drvConfig.devicesCnt = NUM_TOTAL_IC;
  battery.drvConfig.drvInstance = 0U;
  battery.drvConfig.loopBack = false;
  for(uint8_t i = 0; i < NUM_TOTAL_IC; i++){
      battery.drvConfig.device[i] = BCC_DEVICE_MC33771C;
      battery.drvConfig.cellCnt[i] = NUM_CELL_IC;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // print_state(); // prints the current state
    // sdc_reset();
    LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_15);
    BCC_WakeUp(&(battery.drvConfig));

    // proceed unless it's 🔥
    if(state != INIT){

      // SYSTEM CHECK
      read_device_measurements(&battery, true, true);
      bool b_check = battery_check(&battery, true);
      if(b_check == false){
        enqueue(ACU_Status_1, FDCAN1);
        enqueue(ACU_Status_2, FDCAN1);
        #if DEBUGG == 0
        state = state == INIT ? INIT : SHITDOWN;
        #endif
      }
    }
    switch(state){
      case (INIT):
        init();
        break;
      case (STANDBY):
        standby();
        break;
      case (PRECHARGE):
        precharge();
        break;
      case (CHARGE):
        charge();
        break;
      case (NORMAL):
        normal();
        break;
      case (SHITDOWN):
        shitdown();
        break;
      default:
        print_lpuart("ur cooked if you end up here...\n");
        state = SHITDOWN;
        break;
    }
    cps++;
    #if DEBUG_MODE == 0
    if(HAL_GetTick() - prev >= 1000){ // debug every 2 seconds
      prev += 1000;
      debug();
      sprintf(print_buffer, "%ucps\n", cps);
      print_lpuart(print_buffer);
      cps = 0;
    }
    #endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  LL_SPI_Disable(SPI1);
  LL_SPI_Disable(SPI2);
  HAL_FDCAN_Stop(&hfdcan1);
  HAL_FDCAN_Stop(&hfdcan2);
  HAL_FDCAN_Stop(&hfdcan3);
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();
   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  LL_RCC_HSI_SetCalibTrimming(64);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 16, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();
   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1us transition state at intermediate medium speed clock*/
  for (__IO uint32_t i = (170 >> 1); i !=0; i--);

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_SetSystemCoreClock(128000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/// @brief Enable DWT_Delay
/// @param void
void DWT_Delay_Init(void){
  // do we need to check (!CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk)?
  // don't know but it works so I'm not touching this
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;  // Reset counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // Enable counter
}

/// @brief Interrupt handler for RX CAN messages
/// @param hfdcan should always be FDCAN1
/// @param RxFifo0ITs 
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    // Check if a new message is received in FIFO 0
    if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
    {
        // if buffer full, then skip
        if(CAN_RxBufferLevel == 255U) return;

        // Retrieve the message from FIFO 0
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, CAN_RxData) == HAL_OK)
        {
            CAN_RxBuffer[CAN_RxBufferTop].identifier = RxHeader.Identifier;
            CAN_RxBuffer[CAN_RxBufferTop].length = RxHeader.DataLength;
            CAN_RxBuffer[CAN_RxBufferTop].instance = hfdcan->Instance;

            #if SPAMPRINT == 0
            if (CAN_RxBuffer[CAN_RxBufferTop].identifier == Precharge_ACU){
              uint8_t headerBuff[64], dataBuff[8];
              sprintf(headerBuff, "Received CAN message: ID=0x%lX, DLC=%ld, Data=\n", RxHeader.Identifier, RxHeader.DataLength);
              print_lpuart(headerBuff);
              for (uint32_t i = 0; i < RxHeader.DataLength; i++)
              {
                  sprintf(dataBuff, " 0x%02X, ", CAN_RxData[i]);
                  print_lpuart(dataBuff);
                  bzero((void *)dataBuff, 8);
              }
              sprintf(dataBuff, "\n");
              print_lpuart(dataBuff);
              #endif
            }

            // copy the data
            bzero((void *)CAN_RxBuffer[CAN_RxBufferTop].data, 64);
            memcpy((void * restrict)CAN_RxBuffer[CAN_RxBufferTop].data, CAN_RxData, RxHeader.DataLength);
            CAN_RxBufferTop++; // increment and mod the pointers in the buffer
            CAN_RxBufferLevel++;
        }
    }
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
  __disable_irq();
  while (1)
  {
    print_lpuart("Error has occured!\n");
    LL_mDelay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
