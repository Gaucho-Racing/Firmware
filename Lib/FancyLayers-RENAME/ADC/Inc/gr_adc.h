#include "main.h"

#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC

void ADC_Enable_And_Calibrate(ADC_TypeDef *ADC);

// TODO: Make this private
// Internal variable to store which ADC groups have been initialized
uint8_t ADC12_Initialized = 0, ADC345_Initialized = 0;
// Array of the possible ranks a channel can be set to
uint32_t Rank[] = {LL_ADC_REG_RANK_1, LL_ADC_REG_RANK_2, LL_ADC_REG_RANK_3, LL_ADC_REG_RANK_4, 
				   LL_ADC_REG_RANK_5, LL_ADC_REG_RANK_6, LL_ADC_REG_RANK_7, LL_ADC_REG_RANK_8, 
				   LL_ADC_REG_RANK_9, LL_ADC_REG_RANK_10, LL_ADC_REG_RANK_11, LL_ADC_REG_RANK_12, 
				   LL_ADC_REG_RANK_13, LL_ADC_REG_RANK_14, LL_ADC_REG_RANK_15, LL_ADC_REG_RANK_16};
// Array of number of channels that can be initialized
uint32_t Num_Channel_Options[] = {LL_ADC_REG_SEQ_SCAN_DISABLE, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS,
						   LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS};

								
/// @brief Number of clock cycles to be considered one tick of the ADC
typedef enum {
	PS_1 = LL_ADC_CLOCK_ASYNC_DIV1,
	PS_2 = LL_ADC_CLOCK_ASYNC_DIV2,
	PS_4 = LL_ADC_CLOCK_ASYNC_DIV4,
	PS_6 = LL_ADC_CLOCK_ASYNC_DIV6,
	PS_8 = LL_ADC_CLOCK_ASYNC_DIV8,
	PS_10 = LL_ADC_CLOCK_ASYNC_DIV10,
	PS_12 = LL_ADC_CLOCK_ASYNC_DIV12,
	PS_16 = LL_ADC_CLOCK_ASYNC_DIV16,
	PS_32 = LL_ADC_CLOCK_ASYNC_DIV32,
	PS_64 = LL_ADC_CLOCK_ASYNC_DIV64,
	PS_128 = LL_ADC_CLOCK_ASYNC_DIV128,
	PS_256 = LL_ADC_CLOCK_ASYNC_DIV256
} Pre_Scaler_Values;

/// @brief Number of bits used to store the converted data, defines range/resolution of data
typedef enum { 
	RESOLUTION_12 = LL_ADC_RESOLUTION_12B,
	RESOLUTION_10 = LL_ADC_RESOLUTION_10B,
	RESOLUTION_8 = LL_ADC_RESOLUTION_8B,
	RESOLUTION_6 = LL_ADC_RESOLUTION_6B
} Resolution;

/// @brief Struct for pin initialization
typedef struct {
	unsigned long pin;  // Bit mask of pins
	GPIO_TypeDef *port; // Port
} Pin_Ports;

// Overall ADC initialization
void ADC_Init(ADC_Init_Values *Init_Values);

///
/// @brief The struct used to initialize each ADC
///
/// @param ADC The ADC to be initialized (ADC 1 to 5)
/// @param PS_Values Determines the conversion speed of the ADC
/// @param res Determines the resolution/range of data
/// @param Pins Array of pins (with their respective ports) to be initialized 
/// @param Num_Pin_Port_Objs Number of pin port objects to initialize
/// @param Num_Channels Number of ADC channels, must be a value between 1 and 16
/// @param Channels Array of channels to be initialized, DMA output will much the order
/// 					of this array
/// @param SamplingTimes An array of sampling times for the channels, must match the
///							the ordering of the channels array
typedef struct{
	ADC_TypeDef *ADC;
	Pre_Scaler_Values PS_Values;
	Resolution res;
	uint32_t Num_Pin_Port_Objs;
	Pin_Ports *Pins;
	uint32_t Num_Channels;
	Channel *Channels;
	SamplingTime *SamplingTimes;
} ADC_Init_Values;

// TODO: Make these function private
// Initializes an ADC group
void ADC_Group_Init(ADC_TypeDef *ADC, Pre_Scaler_Values PS_Val);

// Initializes each individual ADC
void ADC_Init_Single(ADC_TypeDef *ADC, Resolution res);

// Initialize the channel configurations of the ADC
void ADC_Regular_Group_Init(ADC_TypeDef *ADC, unsigned long Sequence_Length);

// Initialize a single port and all the pins used on that port
void ADC_Init_Pins(Pin_Ports *input);

__extension__ typedef enum {
	ADC_CHANNEL_1 = LL_ADC_CHANNEL_1,
	ADC_CHANNEL_2 = LL_ADC_CHANNEL_2,
	ADC_CHANNEL_3 = LL_ADC_CHANNEL_3,
	ADC_CHANNEL_4 = LL_ADC_CHANNEL_4,
	ADC_CHANNEL_5 = LL_ADC_CHANNEL_5,
	ADC_CHANNEL_6 = LL_ADC_CHANNEL_6,
	ADC_CHANNEL_7 = LL_ADC_CHANNEL_7,
	ADC_CHANNEL_8 = LL_ADC_CHANNEL_8,
	ADC_CHANNEL_9 = LL_ADC_CHANNEL_9,
	ADC_CHANNEL_10 = LL_ADC_CHANNEL_10,
	ADC_CHANNEL_11 = LL_ADC_CHANNEL_11,
	ADC_CHANNEL_12 = LL_ADC_CHANNEL_12,
	ADC_CHANNEL_13 = LL_ADC_CHANNEL_13,
	ADC_CHANNEL_14 = LL_ADC_CHANNEL_14,
	ADC_CHANNEL_15 = LL_ADC_CHANNEL_15,
	ADC_CHANNEL_16 = LL_ADC_CHANNEL_16,
	ADC_CHANNEL_17 = LL_ADC_CHANNEL_17,
	ADC_CHANNEL_18 = LL_ADC_CHANNEL_18,
	VREFINT = LL_ADC_CHANNEL_VREFINT,
	TEMPSENSOR_ADC1 = LL_ADC_CHANNEL_TEMPSENSOR_ADC1,
	TEMPSENSOR_ADC5 = LL_ADC_CHANNEL_TEMPSENSOR_ADC5,
	VBAT = LL_ADC_CHANNEL_VBAT,
	VOPAMP1 = LL_ADC_CHANNEL_VOPAMP1,
	VOPAMP2 = LL_ADC_CHANNEL_VOPAMP2,
	VOPAMP3_ADC2 = LL_ADC_CHANNEL_VOPAMP3_ADC2,
	VOPAMP3_ADC3 = LL_ADC_CHANNEL_VOPAMP3_ADC3,
	VOPAMP4 = LL_ADC_CHANNEL_VOPAMP4,
	VOPAMP5 = LL_ADC_CHANNEL_VOPAMP5,
	VOPAMP6 = LL_ADC_CHANNEL_VOPAMP6,
} Channel;

/// @brief For oversampling, number of times to sample the channel before returning the output
typedef enum {
	SAMPLINGTIME_2CYCLES_5 = LL_ADC_SAMPLINGTIME_2CYCLES_5,
	SAMPLINGTIME_6CYCLES_5 = LL_ADC_SAMPLINGTIME_6CYCLES_5,
	SAMPLINGTIME_12CYCLES_5 = LL_ADC_SAMPLINGTIME_12CYCLES_5,
	SAMPLINGTIME_24CYCLES_5 = LL_ADC_SAMPLINGTIME_24CYCLES_5,
	SAMPLINGTIME_47CYCLES_5 = LL_ADC_SAMPLINGTIME_47CYCLES_5,
	SAMPLINGTIME_92CYCLES_5 = LL_ADC_SAMPLINGTIME_92CYCLES_5,
	SAMPLINGTIME_247CYCLES_5 = LL_ADC_SAMPLINGTIME_247CYCLES_5,
	SAMPLINGTIME_640CYCLES_5 = LL_ADC_SAMPLINGTIME_640CYCLES_5,
} SamplingTime;

// Initialize each channel
void ADC_Channel_Init(ADC_TypeDef *adc, uint32_t rank, Channel channel, SamplingTime time);

typedef enum {
	SYNC_PCLK_DIV1 = LL_ADC_CLOCK_SYNC_PCLK_DIV1,
	SYNC_PCLK_DIV2 = LL_ADC_CLOCK_SYNC_PCLK_DIV2,
	SYNC_PCLK_DIV4 = LL_ADC_CLOCK_SYNC_PCLK_DIV4,
	ASYNC_DIV1 = LL_ADC_CLOCK_ASYNC_DIV1,
	ASYNC_DIV2 = LL_ADC_CLOCK_ASYNC_DIV2,
	ASYNC_DIV4 = LL_ADC_CLOCK_ASYNC_DIV4,
	ASYNC_DIV6 = LL_ADC_CLOCK_ASYNC_DIV6,
	ASYNC_DIV8 = LL_ADC_CLOCK_ASYNC_DIV8,
	ASYNC_DIV10 = LL_ADC_CLOCK_ASYNC_DIV10,
	ASYNC_DIV12 = LL_ADC_CLOCK_ASYNC_DIV12,
	ASYNC_DIV16 = LL_ADC_CLOCK_ASYNC_DIV16,
	ASYNC_DIV32 = LL_ADC_CLOCK_ASYNC_DIV32,
	ASYNC_DIV64 = LL_ADC_CLOCK_ASYNC_DIV64,
	ASYNC_DIV128 = LL_ADC_CLOCK_ASYNC_DIV128,
	ASYNC_DIV256 = LL_ADC_CLOCK_ASYNC_DIV256,
} CommonClock;

void ADC_Set_Common_Clock(ADC_Common_TypeDef *ADC_Common, CommonClock commonClock);
CommonClock ADC_Get_Common_Clock(ADC_Common_TypeDef *ADC_Common);

typedef enum { LOW = LL_DMA_PRIORITY_LOW, MEDIUM = LL_DMA_PRIORITY_MEDIUM, HIGH = LL_DMA_PRIORITY_HIGH, VERYHIGH = LL_DMA_PRIORITY_VERYHIGH } DMA_Priority;

typedef enum {
	DMA_CHANNEL_1 = LL_DMA_CHANNEL_1,
	DMA_CHANNEL_2 = LL_DMA_CHANNEL_2,
	DMA_CHANNEL_3 = LL_DMA_CHANNEL_3,
	DMA_CHANNEL_4 = LL_DMA_CHANNEL_4,
	DMA_CHANNEL_5 = LL_DMA_CHANNEL_5,
	DMA_CHANNEL_6 = LL_DMA_CHANNEL_6,
	DMA_CHANNEL_7 = LL_DMA_CHANNEL_7,
	DMA_CHANNEL_8 = LL_DMA_CHANNEL_8
} DMA_Channel;

void DMA_Init(DMA_TypeDef *DMA, DMA_Channel channel, uint32_t src_address, void* dest_address, uint32_t p_data_size, uint32_t m_data_size, uint32_t num_data, ADC_TypeDef *ADC,
	      DMA_Priority priority);

/*
 */
void ADC_UpdateAnalogValues(uint16_t **adcDataValues, volatile uint16_t *new_values, int num_signals, int window_size, uint16_t *weighted_output);

#endif
