#include "main.h"


#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC

// What the prescaler value 
typedef enum{
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

// Resolution of the ADC in bits
typedef enum{
    RESOLUTION_12 = LL_ADC_RESOLUTION_12B,
    RESOLUTION_10 = LL_ADC_RESOLUTION_10B,
    RESOLUTION_8 =  LL_ADC_RESOLUTION_8B,
    RESOLUTION_6 = LL_ADC_RESOLUTION_6B
} Resolution;

// Data Alignment
typedef enum{
    RIGHT = LL_ADC_DATA_ALIGN_RIGHT,
    LEFT = LL_ADC_DATA_ALIGN_LEFT
} Alignment;

// Initializes an ADC group
void ADC_Group_Init(unsigned long ADC, Pre_Scaler_Values PS_Val);

// Initializes each individual ADC
void ADC_Init(unsigned long ADC, Resolution res, Alignment align);

// Initialize a single port and all the pins used on that port
void ADC_Init_Pins(Pin_Ports *input);

// Initialize the regular channels of the ADC
void ADC_Regular_Group_Init(unsigned long ADC);


// Struct to easily 
typedef struct{
    unsigned long pin; // Bit mask of pins
    unsigned long port; // Port
} Pin_Ports;

/* 3 Init Function
 * 1. Initialize each group: 1&2, 3&4, 5
 * 2. Initialize each ADC
 * 4. Initialize each pin in the ADC
 */

// Return a pointer to DMA 
#endif
