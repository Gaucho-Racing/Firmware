#include "adc.h"
#include "main.h"

void ADC_Group_Init(unsigned long ADC, Pre_Scaler_Values PS_Val){
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    ADC_CommonInitStruct.CommonClock = PS_Val;
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
    if (ADC > ADC_2){
        LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC3), &ADC_CommonInitStruct);
    }
    else{
        LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);          
    }
}

void ADC_Init(unsigned long ADC, Resolution res, Alignment align){
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.Resolution = res;
	ADC_InitStruct.DataAlignment = align;
	ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADC, &ADC_InitStruct);
}

void ADC_Regular_Group_Init(unsigned long ADC){
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_PRESERVED;
	LL_ADC_REG_Init(ADC, &ADC_REG_InitStruct);
}

void ADC_Init_Pins(Pin_Ports *input){
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = input->pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(input->port, &GPIO_InitStruct);
}

