#include "adc.h"
#include "main.h"

void ADC_Group_Init(ADC_Number ADC, Pre_Scaler_Values PS_Val){
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    ADC_CommonInitStruct.CommonClock = PS_Val;
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
    switch(ADC){
        case ADC_1:
        case ADC_2:
            LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
            break;
        
        case ADC_3:
        case ADC_4:
        case ADC_5:
            LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC3), &ADC_CommonInitStruct);
    }
}

void ADC_Init(ADC_Number ADC, Resolution res, Alignment align){
    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.Resolution = res;
	ADC_InitStruct.DataAlignment = align;
	ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
    switch(ADC){
        case ADC_1:
            LL_ADC_Init(ADC1, &ADC_InitStruct);
        case ADC_2:
            LL_ADC_Init(ADC2, &ADC_InitStruct);
        case ADC_3:
            LL_ADC_Init(ADC3, &ADC_InitStruct);
        case ADC_4:
            LL_ADC_Init(ADC5, &ADC_InitStruct);
        case ADC_5:
            LL_ADC_Init(ADC5, &ADC_InitStruct);
        
    }
}


GPIO_InitStruct.Pin = BSPD_SIGNAL_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(BSPD_SIGNAL_GPIO_Port, &GPIO_InitStruct);


