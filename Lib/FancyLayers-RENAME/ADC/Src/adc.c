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
