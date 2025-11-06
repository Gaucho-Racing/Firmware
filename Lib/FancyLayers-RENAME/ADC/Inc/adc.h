#include<stdint.h>

#ifndef GR_PERIPHERALS_ADC
#define GR_PERIPHERALS_ADC

typedef struct ADCHandler{
    uint8_t ADC_Num; // Which ADC to use
    uint8_t numChannels; // Number of Channels to initialize
    // Maybe what clock to use?
};

#endif