#include "main.h"


#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC

enum ADC_Group {
    GROUP_12,
    GROUP_34,
    GROUP_5
};

enum Pre_Scaler_Values{
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
};

void ADC_Group_Init(enum ADC_Group group, enum Pre_Scaler_Values PS_Val);

enum ADC_Number {
    ADC_1,
    ADC_2,
    ADC_3,
    ADC_4,
    ADC_5
};

enum Bit_Alignment{
    RIGHT,
    LEFT
};

// Return a pointer to DMA 

typedef struct ADC_Initializer{
    enum ADC_Number ADC_Num; // ADC to use
    enum Bit_Alignment Bit_Align; // The bit alignment for the ADC output
};

// Input for initializing a single pin
typedef struct ADCPinInitializer{
    GPIO_TypeDef *GPIOx; // GPIO Bus
    uint16_t GPIO_Pin; // Pin number
};

/* 3 Init Function
 * 1. Initialize each group: 1&2, 3&4, 5
 * 2. Initialize each ADC
 * 4. Initialize each pin in the ADC
 */
#endif