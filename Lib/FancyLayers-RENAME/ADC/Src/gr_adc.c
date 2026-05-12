/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    adc.c
 * @brief   This file provides code for the configuration
 *          of the ADC instances.
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "gr_adc.h"

#include "Logomatic.h"
#include "main.h"

// Initializes an ADC group
void ADC_Group_Init(ADC_TypeDef *ADC, Pre_Scaler_Values PS_Val);

// Initializes each individual ADC
void ADC_Init_Single(ADC_TypeDef *ADC, Resolution res);

// Initialize the channel configurations of the ADC
void ADC_Regular_Group_Init(ADC_TypeDef *ADC, unsigned long Sequence_Length);

// Initialize a single port and all the pins used on that port
void ADC_Init_Pins(Pin_Ports *input);

// Initialize each channel
void ADC_Channel_Init(ADC_TypeDef *adc, uint32_t rank, Channel channel, SamplingTime time);

// Internal variables
// Store ADC groups have been initialized
uint8_t ADC12_Initialized = 0, ADC345_Initialized = 0;
// Array of the possible ranks a channel can be set to
uint32_t Rank[] = {LL_ADC_REG_RANK_1, LL_ADC_REG_RANK_2,  LL_ADC_REG_RANK_3,  LL_ADC_REG_RANK_4,  LL_ADC_REG_RANK_5,  LL_ADC_REG_RANK_6,  LL_ADC_REG_RANK_7,  LL_ADC_REG_RANK_8,
		   LL_ADC_REG_RANK_9, LL_ADC_REG_RANK_10, LL_ADC_REG_RANK_11, LL_ADC_REG_RANK_12, LL_ADC_REG_RANK_13, LL_ADC_REG_RANK_14, LL_ADC_REG_RANK_15, LL_ADC_REG_RANK_16};
// Array of number of channels that can be initialized
uint32_t Num_Channel_Options[] = {LL_ADC_REG_SEQ_SCAN_DISABLE,	      LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
				  LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS,
				  LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS,  LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS,
				  LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS, LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS};

void ADC_Init(ADC_Init_Values *Init_Values)
{
	// Initialize the ADC Common Group
	uint8_t group_initialized = 0; // For checking if the group is already initialized

	// ADC Group 12 already initialized
	if (__LL_ADC_COMMON_INSTANCE(Init_Values->ADC) == __LL_ADC_COMMON_INSTANCE(ADC1) && ADC12_Initialized) {
		LOGOMATIC("ADC Group 12 already initialized");
		group_initialized = 1;
	}

	// ADC Group 345 already initialized
	if (__LL_ADC_COMMON_INSTANCE(Init_Values->ADC) == __LL_ADC_COMMON_INSTANCE(ADC3) && ADC345_Initialized) {
		LOGOMATIC("ADC Group 345 already initialized");
		group_initialized = 1;
	}
	if (!group_initialized) {
		ADC_Group_Init(Init_Values->ADC, Init_Values->PS_Value);
	}

	// Initialize the individual ADCs
	ADC_Init_Single(Init_Values->ADC, Init_Values->Res);

	// Initialize regular channels for individual ADC
	ADC_Regular_Group_Init(Init_Values->ADC, Num_Channel_Options[Init_Values->Num_Channels - 1]);

	// Initialize all pins
	for (uint32_t i = 0; i < Init_Values->Num_Pin_Port_Objs; ++i) {
		ADC_Init_Pins(&(Init_Values->Pins[i]));
	}

	// Initialize Channels
	for (uint32_t i = 0; i < Init_Values->Num_Channels; ++i) {
		ADC_Channel_Init(Init_Values->ADC, Rank[i], Init_Values->Channels[i], Init_Values->SamplingTimes[i]);
	}
}

void ADC_Group_Init(ADC_TypeDef *ADC, Pre_Scaler_Values PS_Val)
{
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
	ADC_CommonInitStruct.CommonClock = PS_Val;
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT; // ADC Dual Mode disabled
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC), &ADC_CommonInitStruct);
}

void ADC_Init_Single(ADC_TypeDef *ADC, Resolution res)
{
	LL_ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.Resolution = res;
	ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT; // Right Align: LSB at bit 0, directly translates to numerical value
	ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;	// No ADC low power mode
	LL_ADC_Init(ADC, &ADC_InitStruct);
}

void ADC_Regular_Group_Init(ADC_TypeDef *ADC, unsigned long Sequence_Length)
{
	LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE; // ADC conversion is triggered by software
	ADC_REG_InitStruct.SequencerLength = Sequence_Length;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE; // No interrupts
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;	      // Continuous conversion
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;   // Allows unlimited DMA transfer of regular group data
	ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;	      // Allows data to be overwritten when the buffer fills
	LL_ADC_REG_Init(ADC, &ADC_REG_InitStruct);
}

void ADC_Init_Pins(Pin_Ports *input)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = input->pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(input->port, &GPIO_InitStruct);
}

void ADC_Channel_Init(ADC_TypeDef *ADC, uint32_t rank, Channel channel, SamplingTime time)
{
	LL_ADC_REG_SetSequencerRanks(ADC, rank, channel);
	LL_ADC_SetChannelSamplingTime(ADC, channel, time);
	LL_ADC_SetChannelSingleDiff(ADC, channel, LL_ADC_SINGLE_ENDED);
}

// Enable ADC with proper calibration and voltage regulator handling
void ADC_Enable_And_Calibrate(ADC_TypeDef *ADC)
{
	LL_ADC_StartCalibration(ADC, LL_ADC_SINGLE_ENDED);
	while (LL_ADC_IsCalibrationOnGoing(ADC))
		;

	LL_ADC_Enable(ADC);
	while (!LL_ADC_IsEnabled(ADC))
		;

	// Enable voltage regulator after ADC enable
	ADC->CR |= ADC_CR_ADVREGEN;
	ADC->CR &= ~ADC_CR_DEEPPWD;
	LL_mDelay(1);

	LL_ADC_REG_StartConversion(ADC);
}

void DMA_Init(DMA_Init_Values *Init_Values)
{
	LL_DMA_InitTypeDef config = {0};
	config.PeriphOrM2MSrcAddress = Init_Values->Src_Address;
	config.MemoryOrM2MDstAddress = (uint32_t)Init_Values->Dest_Address;
	config.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;	  // Direction of data transfer: from peripheral to memory
	config.Mode = LL_DMA_MODE_CIRCULAR;			  // Circular mode: continuously transfers after the last cycle finishes
	config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT; // Peripheral memory address doesn't increments - all results written to the ADCx->DR register
	config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;	  // Memory address increments after every transfer
	config.NbData = Init_Values->Num_Data;			  // Transfers num data unit at a time
	switch (Init_Values->Data_Size) {
		case (Byte):
			config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
			config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
			break;
		case (Half_Word):
			config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
			config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
			break;
		case (Word):
			config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
			config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
	}

	// Select ADC DMAMUX request
	if (Init_Values->ADC == ADC1) {
		config.PeriphRequest = LL_DMAMUX_REQ_ADC1;
	} else if (Init_Values->ADC == ADC2) {
		config.PeriphRequest = LL_DMAMUX_REQ_ADC2;
	} else if (Init_Values->ADC == ADC3) {
		config.PeriphRequest = LL_DMAMUX_REQ_ADC3;
	} else if (Init_Values->ADC == ADC4) {
		config.PeriphRequest = LL_DMAMUX_REQ_ADC4;
	} else if (Init_Values->ADC == ADC5) {
		config.PeriphRequest = LL_DMAMUX_REQ_ADC5;
	}

	config.Priority = Init_Values->Priority;
	LL_DMA_Init(Init_Values->DMA, Init_Values->Channel, &config);
}

// NOTE: DMA init is still using NOINCREMENT
// TODO: Add int n to consider last n values
void ADC_UpdateAnalogValues(uint16_t **adcDataValues, volatile uint16_t *new_values, int num_signals, int window_size, uint16_t *weighted_output)
{
	static int num = 0;
	static uint8_t filled = 0;
	for (int i = 0; i < num_signals; ++i) {
		weighted_output[i] += (new_values[i] - (filled ? adcDataValues[i][num] : 0)) / window_size; // Update the average
		adcDataValues[i][num] = new_values[i];
	}

	if (++num >= window_size) {
		num = 0;
		filled = 1;
	}
}

/*
EMA
out = out + alpha * (new - out)
void ADC_WeightedOutput(uint16_t *latest, float *weighted_output, int num_signals, float alpha) {
    for (int i = 0; i < num_signals; ++i){
		weighted_output[i] += ((float)latest[i] - weighted_output) * alpha;
	}
}
*/

void ADC_UpdateAnalogValues_EMA(volatile uint16_t *new_values, int num_signals, double alpha, uint16_t *weighted_output)
{
	for (int i = 0; i < num_signals; ++i) {
		weighted_output[i] = alpha * new_values[i] + (1 - alpha) * weighted_output[i];
	}
}

// void updateAnalogInputs(void)
// {
//     float newValue;

//     for (uint8_t sig = AUX_SIGNAL; sig <= STEERING_ANGLE; sig++)
//     {
//         newValue = (float)analogRead(sig);
//         adcSumValues[sig] -= adcDataValues[sig][readIndex];
//         adcDataValues[sig][readIndex] = newValue;
//         adcSumValues[sig] += newValue;
//         ((float*)&globalAnalog)[sig] = adcSumValues[sig] / WINDOW_SIZE;
//     }

//     readIndex = (readIndex + 1) % WINDOW_SIZE;
// }
