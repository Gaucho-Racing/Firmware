#include "main.h"

#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC

//--------------------------------------ADC Initialization-------------------------------------

void ADC_Enable_And_Calibrate(ADC_TypeDef *ADC);
								
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

/// @brief The struct used to initialize each ADC
///
/// @param ADC The ADC to be initialized (ADC 1 to 5)
/// @param PS_Values Determines the conversion speed of the ADC
/// @param res Determines the resolution/range of data
/// @param Pins Array of pins (with their respective ports) to be initialized 
/// @param Num_Pin_Port_Objs Number of pin port objects to initialize
/// @param Num_Channels Number of ADC channels, must be a value between 1 and 16
/// @param Channels Array of channels to be initialized, DMA output will match the order
/// 					of this array
/// @param SamplingTimes Array of channel sampling times, should align with channels array
typedef struct{
	ADC_TypeDef *ADC;
	Pre_Scaler_Values PS_Value;
	Resolution res;
	uint32_t Num_Pin_Port_Objs;
	Pin_Ports *Pins;
	uint32_t Num_Channels;
	Channel *Channels;
	SamplingTime *SamplingTimes;
} ADC_Init_Values;

// Main ADC initialization function
void ADC_Init(ADC_Init_Values *Init_Values);

//--------------------------------------DMA Initialization-------------------------------------
/// @brief Priority of DMA transfer
typedef enum { 
	LOW = LL_DMA_PRIORITY_LOW, 
	MEDIUM = LL_DMA_PRIORITY_MEDIUM,
	HIGH = LL_DMA_PRIORITY_HIGH,
	VERYHIGH = LL_DMA_PRIORITY_VERYHIGH
} DMA_Priority;

/// @brief DMA channels
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

/// @brief The data size of the data to be transfered by DMA
typedef enum {
	Byte,
	Half_Word,
	Word
} DMA_Data_Size;

/// @brief The struct used to initialize each DMA
///
/// @param DMA The DMA to be initialized
/// @param ADC The ADC instance this DMA channel handles
/// @param Channels DMA channel to initialize - each channel handles a single instance of ADC
/// @param Src_Address Source address, use LL_ADC_DMA_GetRegAddr() to get the address
/// @param Dest_Addresses Pointers to destination buffer
/// @param Data_Size Array of data sizes, can be options of DMA_Data_Size
/// @param Priority Array of priorities to set the priorities of each DMA channel
typedef struct{
	DMA_TypeDef *DMA;
	ADC_TypeDef *ADC;
	DMA_Channel Channel;
	uint32_t Src_Address;
	void *Dest_Address;
	DMA_Data_Size Data_Size;
	DMA_Priority Priority;
} DMA_Init_Values;

void DMA_Init(DMA_Init_Values *Init_Values);

/*
 */
void ADC_UpdateAnalogValues(uint16_t **adcDataValues, volatile uint16_t *new_values, int num_signals, int window_size, uint16_t *weighted_output);

void ADC_UpdateAnalogValues_EMA(volatile uint16_t *new_values,
                                int num_signals,
                                int window_size,
                                uint16_t *weighted_output);

#endif
