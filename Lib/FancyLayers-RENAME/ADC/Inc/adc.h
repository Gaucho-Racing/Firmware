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

// How many ranks to enable
typedef enum{
    NO_RANKS = LL_ADC_REG_SEQ_SCAN_DISABLE,
	RANKS_2 = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
    RANKS_3 = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,
    RANKS_4 = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
    RANKS_5 = LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,
    RANKS_6 = LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,
    RANKS_7 = LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS,
    RANKS_8 = LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS,
    RANKS_9 = LL_ADC_REG_SEQ_SCAN_ENABLE_9RANKS,
    RANKS_10 = LL_ADC_REG_SEQ_SCAN_ENABLE_10RANKS,
    RANKS_11 = LL_ADC_REG_SEQ_SCAN_ENABLE_11RANKS,
    RANKS_12 = LL_ADC_REG_SEQ_SCAN_ENABLE_12RANKS,
    RANKS_13 = LL_ADC_REG_SEQ_SCAN_ENABLE_13RANKS,
    RANKS_14 = LL_ADC_REG_SEQ_SCAN_ENABLE_14RANKS,
    RANKS_15 = LL_ADC_REG_SEQ_SCAN_ENABLE_15RANKS,
    RANKS_16 = LL_ADC_REG_SEQ_SCAN_ENABLE_16RANKS   
} NUM_RANKS;

// Initialize the channel configurations of the ADC
void ADC_Regular_Group_Init(unsigned long ADC, NUM_RANKS ranks);

// How many ranks to enable
typedef enum{
    RANK_1 = LL_ADC_REG_RANK_1,
    RANK_2 = LL_ADC_REG_RANK_2,
    RANK_3 = LL_ADC_REG_RANK_3,
    RANK_4 = LL_ADC_REG_RANK_4,
    RANK_5 = LL_ADC_REG_RANK_5,
    RANK_6 = LL_ADC_REG_RANK_6,
    RANK_7 = LL_ADC_REG_RANK_7,
    RANK_8 = LL_ADC_REG_RANK_8,
    RANK_9 = LL_ADC_REG_RANK_9,
    RANK_10 = LL_ADC_REG_RANK_10,
    RANK_11 = LL_ADC_REG_RANK_11,
    RANK_12 = LL_ADC_REG_RANK_12,
    RANK_13 = LL_ADC_REG_RANK_13,
    RANK_14 = LL_ADC_REG_RANK_14,
    RANK_15 = LL_ADC_REG_RANK_15,
    RANK_16 = LL_ADC_REG_RANK_16
} RANK;


// Initialize each channel
void ADC_Channel_Init();

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
