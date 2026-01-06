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

#include "adc.h"

#include "main.h"

ADC_TypeDef *GetADC(unsigned long adc)
{
	switch (adc) {
		case 1:
			return ADC1;
		case 2:
			return ADC2;
		case 3:
			return ADC3;
		case 4:
			return ADC4;
		case 5:
			return ADC5;
	}
}

void ADC_Group_Init(unsigned long ADC, Pre_Scaler_Values PS_Val)
{
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
	ADC_CommonInitStruct.CommonClock = PS_Val;
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
	if (ADC > ADC_2) {
		LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC3),
				  &ADC_CommonInitStruct);
	} else {
		LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1),
				  &ADC_CommonInitStruct);
	}
}

void ADC_Init(unsigned long ADC, Resolution res, Alignment align)
{
	LL_ADC_InitTypeDef ADC_InitStruct = {0};
	ADC_InitStruct.Resolution = res;
	ADC_InitStruct.DataAlignment = align;
	ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
	LL_ADC_Init(GetADC(ADC), &ADC_InitStruct);
}

void ADC_Regular_Group_Init(unsigned long ADC, NumRanks ranks)
{
	LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REG_InitStruct.SequencerLength = rank;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
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

void ADC_Channel_Init(unsigned long adc, Rank rank, Channel channel,
		      ChannelSingleDiff diff, SamplingTime time)
{
	LL_ADC_REG_SetSequencerRanks(GetADC(adc), rank, channel);
	LL_ADC_SetChannelSamplingTime(GetADC(adc), channel, time);
	LL_ADC_SetChannelSingleDiff(GetADC(adc), channel, diff);
}

void ADC_Set_Common_Clock(ADC_Common_TypeDef *ADC_Common,
			  CommonClock commonClock)
{
	LL_ADC_SetCommonClock(ADC_Common, commonClock);
}

CommonClock ADC_Get_Common_Clock(ADC_Common_TypeDef *ADC_Common)
{
	return LL_ADC_GetCommonClock(ADC_Common);
}

void DMA_Init(DMA_TypeDef *DMA, uint32_t channel, LL_DMA_InitTypeDef *config) {
	LL_DMA_Init (DMA, channel, config);
}