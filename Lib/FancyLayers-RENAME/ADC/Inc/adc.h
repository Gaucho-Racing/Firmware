#include "main.h"


#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC


/*typedef struct {
    // ADC1
    float AUX_SIGNAL;
    float BSPD_SIGNAL;
    float APPS1_SIGNAL;
    float APPS2_SIGNAL;
    float BRAKE_R_SIGNAL;
    float BRAKE_F_SIGNAL;
    // ADC2
    float IMD_SENSE;
    float AMS_SENSE;
    float BSE_SIGNAL;
    float BSPD_SENSE;
    float STEERING_ANGLE;
} AnalogInput;
typedef enum {
    // ADC1
    AUX_SIGNAL = 0,
    BSPD_SIGNAL,
    APPS1_SIGNAL,
    APPS2_SIGNAL,
    BRAKE_R_SIGNAL,
    BRAKE_F_SIGNAL,
    // ADC2
    IMD_SENSE,
    AMS_SENSE,
    BSE_SIGNAL, // Who knows
    BSPD_SENSE,
    STEERING_ANGLE,
} AnalogSignal;

void updateAnalogInputs(void);

extern AnalogInput globalAnalog;

const uint32_t ADC1_COUNT = 6;
unsigned long ADC1_pins[ADC1_COUNT] = {BSPD_SIGNAL_Pin,	 APPS1_SIGNAL_Pin,
			     APPS2_SIGNAL_Pin,	 BRAKE_F_SIGNAL_Pin,
			     BRAKE_R_SIGNAL_Pin, AUX_SIGNAL_Pin};
unsigned long ADC1_ports[ADC1_COUNT] = {BSPD_SIGNAL_GPIO_Port, APPS1_SIGNAL_GPIO_Port, APPS2_SIGNAL_GPIO_Port, BRAKE_F_SIGNAL_GPIO_Port, BRAKE_R_SIGNAL_GPIO_Port, AUX_SIGNAL_GPIO_Port};

const uint32_t ADC2_COUNT = 5;
unsigned long ADC2_pins[ADC2_COUNT] = {BSE_SIGNAL_Pin, BSPD_SENSE_Pin, IMD_SENSE_Pin,
			     AMS_SENSE_Pin, STEERING_ANGLE_Pin};
unsigned long ADC2_ports[ADC2_COUNT] = {BSE_SIGNAL_GPIO_Port, BSPD_SENSE_GPIO_Port, IMD_SENSE_GPIO_Port, AMS_SENSE_GPIO_Port, STEERING_ANGLE_GPIO_Port};
*/

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

// Which ADC to initialize
typedef enum{
    ADC_1,
    ADC_2,
    ADC_3,
    ADC_4,
    ADC_5,
} ADC_Number;


typedef enum{
    RESOLUTION_12 = LL_ADC_RESOLUTION_12B,
    RESOLUTION_10 = LL_ADC_RESOLUTION_10B,
    RESOLUTION_8 =  LL_ADC_RESOLUTION_8B,
    RESOLUTION_6 = LL_ADC_RESOLUTION_6B
} Resolution;

// Initializes an ADC group
void ADC_Group_Init(ADC_Number ADC, Pre_Scaler_Values PS_Val);

void ADC_Init(ADC_Number ADC, Resolution res, );

/* 3 Init Function
 * 1. Initialize each group: 1&2, 3&4, 5
 * 2. Initialize each ADC
 * 4. Initialize each pin in the ADC
 */

// Return a pointer to DMA 
#endif