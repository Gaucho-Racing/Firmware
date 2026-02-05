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

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_crs.h"
#include "stm32g4xx_ll_dma.h"
#include "stm32g4xx_ll_exti.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_lpuart.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_DMA_Init(void);

//static void MX_TIM2_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


//SPI PROBABLY NEEDS TO BE REWORKED, LOOK AT TIM FIRST
/**
 * 
 * @brief Each WS2812B bit corresponds to 6 SPI bits
 * @param val The 8 bits to convert to 3 half-words
 * @param out_arr The location to store the 3 half-words
 */
#define SPI_BITS_TO_LED 6
short WS2812B_SPI_Encoding(uint8_t val, uint16_t* out_arr) {
  uint8_t mask = 0b1 << 7;
  
  int words = 0; 
  int bits_used = 0; 

  uint16_t buff = 0; 
  for (int i = 0; i < 8; i++) {
    // Each bit of 'val' becomes a 6-bit encoding
    uint16_t encoding = (val & mask) ? 0b111100 : 0b110000;

    // Remaining bits in the current buffer
    int bits_left = 16 - bits_used;

    if (bits_left >= 6) {
        // Fits fully in current buffer
        buff |= (encoding << (bits_left - 6));
        bits_used += 6;
    } else {
        // Split across current and next word
        buff |= (encoding >> (6 - bits_left));   // upper part into remaining bits
        out_arr[words++] = buff;                 // write full word

        // carry remainder into next buffer (MSBs)
        buff = (encoding << (16 - (6 - bits_left))) & 0xFFFF;
        bits_used = (6 - bits_left);
    }

        // If current buffer filled, push it out
        if (bits_used == 16) {
            out_arr[words++] = buff;
            buff = 0;
            bits_used = 0;
        }

        mask >>= 1;
    }

    // If any bits remain in buffer at end, flush them
    //if (bits_used > 0) out_arr[words++] = buff;
}
/**
 * @brief
 *  SPI Freq = 2.5 MHz -> 0.4 us
 *  WS2812B Bit Period = 1.2 us
 *  3 SPI bits per WS2812B 
 *  for a single LED (24 RGB bits):
 *    6*24 / 16 = 9 half-words
 *        3 *24 / 8 can rework to use 9 bytes
 * 
 *  Takes in a pointer to arr of 9 half-words and computes the packed format
 * 
 * @param R Red - 0-255
 * @param G Green - 0-255
 * @param B Blue - 0-255
 * @param brightness 0-100
 * @param arr pointer to an array of 9 half-words
 */
void WS2812B_writeRGB_SPI(uint8_t R, uint8_t G, uint8_t B, uint8_t brightness, uint16_t *arr) {
  brightness = (brightness <=100) ? brightness : 100;
  
  R = R * ((float)brightness/100);
  G = G * ((float)brightness/100);
  B = B * ((float)brightness/100); 

  uint16_t out_arr[3]; 
  
  int j = 0; 
  WS2812B_SPI_Encoding(G, out_arr);
  for (int i = 0; i < 3; i++) arr[j++] = out_arr[i]; 
  
  WS2812B_SPI_Encoding(R, out_arr);
  for (int i = 0; i < 3; i++) arr[j++] = out_arr[i]; 
  
  WS2812B_SPI_Encoding(B, out_arr);
  for (int i = 0; i < 3; i++) arr[j++] = out_arr[i]; 
}


// Encode a single 8-bit value into an array for TIM->CCR1 DMA
void WS2812B_TIM_Encoding(uint8_t val, uint16_t *out_arr) {
    // Each bit becomes one uint16_t in the output array
    // Bit 1 -> 32, Bit 0 -> 16
    for (int i = 0; i < 8; i++) { //figure out waveform made via values
        out_arr[i] = (val & (1 << (7 - i))) ? 0 : 41; // 1 or 0 duty
    }
}

// Write an RGB triple into a TIM DMA array (24 bits per LED)
void WS2812B_writeRGB_TIM(uint8_t R, uint8_t G, uint8_t B, uint8_t brightness, uint16_t *arr) {
    brightness = (brightness > 100) ? 100: brightness; 

    uint8_t r = (R * brightness) / 100;
    uint8_t g = (G * brightness) / 100;
    uint8_t b = (B * brightness) / 100;

    // WS2812B expects **GRB order**
    // Each call writes 8 elements into arr
    WS2812B_TIM_Encoding(g, &arr[0]);
    WS2812B_TIM_Encoding(r, &arr[8]);
    WS2812B_TIM_Encoding(b, &arr[16]);
    // Total of 24 elements in arr per LED
}

#define NUM_LEDS 100
#define RESET_PIXELS 20 
#define BITS_PER_LED 24
#define LED_TIM_BUFFER_LEN ((NUM_LEDS + RESET_PIXELS) * BITS_PER_LED)
uint16_t led_tim_data[LED_TIM_BUFFER_LEN]; 
void WS2812B_TIM_SendFrame(void) {
    // --- 1. Enable clocks ---
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    // --- 2. GPIO setup (PA0 = TIM2_CH1) ---
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_0, LL_GPIO_AF_1);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

    // --- 3. Timer setup (PWM, 800kHz bit rate = 1.25 µs) ---
    LL_TIM_DisableCounter(TIM2);
    LL_TIM_SetPrescaler(TIM2, 1); //1
    LL_TIM_SetAutoReload(TIM2, 212); //212
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableARRPreload(TIM2);
    LL_TIM_SetUpdateSource(TIM2, LL_TIM_UPDATESOURCE_REGULAR);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);  // DMA triggers on overflow
    LL_TIM_SetCounter(TIM2, 0);



    // --- 4. DMA setup (Normal mode, not circular) ---
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_ConfigTransfer(DMA1,
                          LL_DMA_CHANNEL_1,
                          LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                          LL_DMA_MODE_NORMAL |                 // stop after one transfer
                          LL_DMA_PERIPH_NOINCREMENT |
                          LL_DMA_MEMORY_INCREMENT |
                          LL_DMA_PDATAALIGN_WORD |
                          LL_DMA_MDATAALIGN_HALFWORD |
                          LL_DMA_PRIORITY_HIGH);

    LL_DMA_ConfigAddresses(DMA1,
                           LL_DMA_CHANNEL_1,
                           (uint32_t)led_tim_data,
                           (uint32_t)&TIM2->CCR1,
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, LED_TIM_BUFFER_LEN);

    // --- 5. DMAMUX: connect TIM2 update request to DMA channel 1 ---
    LL_DMAMUX_SetRequestID(DMAMUX1, LL_DMAMUX_CHANNEL_0, LL_DMAMUX_REQ_TIM2_UP);

    // --- 6. Clear DMA flags ---
    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_TE1(DMA1);

    DMA1->IFCR |= 0; //NOT IMPORTANT, not using interrupts

    // --- 7. Enable DMA + start transfer ---
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_TIM_EnableDMAReq_UPDATE(TIM2);
    LL_TIM_GenerateEvent_UPDATE(TIM2);
    LL_TIM_EnableCounter(TIM2);

    // --- 8. Wait for completion ---
    while (!LL_DMA_IsActiveFlag_TC1(DMA1));  // wait until done

    // --- 9. Disable timer + DMA ---
    LL_TIM_DisableCounter(TIM2);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    //LL_TIM_Disable
}


uint16_t led_spi_data[];
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  //MX_GPIO_Init();
  //MX_DMA_Init();
  //MX_TIM2_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t spi_freq = HAL_RCC_GetPCLK1Freq();
  //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); // 75% duty cycle
  //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  //uint16_t led[9]; 
  //uint8_t reset = 0; 
  //WS2812B_writeRGB_SPI(255, 0, 0, 50, led);
    
  // *SPIx = (&hspi2)->Instance;
  //LL_SPI_Enable(SPIx);

  //50 / 1.25 = 40
  //round up to 48 -> 48/24 = 2 pixels


  //#define USE_SPI
  #define USE_TIM

  #ifdef USE_TIM
 // =================================== FRAME DEFINITION =========================
  //LEDS
  for (int i = 0; i < NUM_LEDS; i++) {
      switch (i % 3) {
        case 0:
          WS2812B_writeRGB_TIM(255,0,0,10,led_tim_data+24*i);
          break;
        case 1:
          WS2812B_writeRGB_TIM(255,0,0,10,led_tim_data+24*i);
          break;

        case 2:
          WS2812B_writeRGB_TIM(255,0,0,10,led_tim_data+24*i);
      }
  }
  //RESET Pixels
  for (int i = NUM_LEDS*24; i < LED_TIM_BUFFER_LEN; i++) led_tim_data[i] = 0;

  // ================================= INITIALIZATION ====================
  /*// Enable clocks
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

  // GPIO for TIM2_CH1
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_0, LL_GPIO_AF_1);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_0, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_0, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

  // Timer settings
  LL_TIM_EnableARRPreload(TIM2);
  LL_TIM_SetPrescaler(TIM2, 0);
  LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
  LL_TIM_SetAutoReload(TIM2, 100-1);

  //LL_TIM_SetAutoReload(TIM2, 50-1);
  //for (int i = 0; i < 24; i ++) led_tim[i] = 5; 

  LL_TIM_SetCounter(TIM2, 0);
  LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
  LL_TIM_OC_EnablePreload(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
  LL_TIM_SetUpdateSource(TIM2, LL_TIM_UPDATESOURCE_REGULAR);
  LL_TIM_EnableDMAReq_UPDATE(TIM2);  // DMA triggers on overflow

  // 5️⃣ DMA channel 1 setup
  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
  while(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1));
  LL_DMA_ConfigTransfer(DMA1,
                          LL_DMA_CHANNEL_1,
                          LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                          LL_DMA_MODE_NORMAL |
                          LL_DMA_PERIPH_NOINCREMENT |
                          LL_DMA_MEMORY_INCREMENT |
                          LL_DMA_PDATAALIGN_WORD |     //VERY IMPORTANT, VERY STRANGE BEHAVIOUR UNLESS USE EXACTLY THIS COMBINATION
                          LL_DMA_MDATAALIGN_HALFWORD | //
                          LL_DMA_PRIORITY_HIGH);

  LL_DMA_ConfigAddresses(DMA1,
                           LL_DMA_CHANNEL_1,
                           (uint32_t) led_data,
                           (uint32_t)&TIM2->CCR1,
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, sizeof(led_data)/sizeof(led_data[0]));

  // 4️⃣ DMAMUX: TIM2 update → DMA1 channel 1
  LL_DMAMUX_SetRequestID(DMAMUX1, LL_DMAMUX_CHANNEL_0, LL_DMAMUX_REQ_TIM2_UP);
  //VERY IMPORTANT TO SET THE MULTIPLEXER CORRECTLY

  DMA1->IFCR |= 0; //NOT IMPORTANT, not using interrupts

  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1); //very important

  //Enable Timer
  LL_TIM_GenerateEvent_UPDATE(TIM2); // ensure preload registers are loaded
  //NOT SURE THIS IS IMPORTANT

  //LL_TIM_OC_SetCompareCH1(TIM2, 32); //manually set the duty cycle of the waveform, not needed with DMA
  LL_TIM_EnableCounter(TIM2);
*/

  WS2812B_TIM_SendFrame();
  #endif

  while (1)
  {
    #ifdef USE_SPI
    HAL_SPI_Transmit(&hspi2, (uint8_t*) led_spi_data, 9, 1000);
    #endif
  }
}

    

    /*
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
    while(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1));

    LL_DMA_ClearFlag_TC1(DMA1);
    LL_DMA_ClearFlag_HT1(DMA1);
    LL_DMA_ClearFlag_TE1(DMA1);

    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)led);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_1, (uint32_t)& (SPIx->DR) );
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, sizeof(led)/sizeof(uint16_t));
    
    //start transfer
    LL_SPI_EnableDMAReq_TX(SPIx);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    // Wait for DMA channel transfer complete
    while(!LL_DMA_IsActiveFlag_TC1(DMA1));
    while(LL_SPI_IsActiveFlag_BSY(SPIx));
    */

    
  

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

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */

static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT; 
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
/*static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  /*TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  /*htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100 - 1 ;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  //HAL_TIM_MspPostInit(&htim2);*/

//}
/*
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  //__HAL_RCC_DMAMUX1_CLK_ENABLE();
  //__HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  /*HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMAMUX_OVR_IRQn interrupt configuration */
  /*HAL_NVIC_SetPriority(DMAMUX_OVR_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMAMUX_OVR_IRQn);

}*/

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
 /*
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
 // __HAL_RCC_GPIOA_CLK_ENABLE();
  //__HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
//}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */