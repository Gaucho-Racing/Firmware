#ifndef PROFILE_H
#define PROFILE_H

#define PROFILE 1

#include <stdint.h>
#include "Logomatic.h"

extern volatile uint32_t PROFILE_AVG_RX_CYCLES;
extern volatile uint32_t PROFILE_RX_SAMPLES;
extern volatile uint32_t ELAPSED_CYCLES;
extern volatile float AVG;


#define START_COUNT \
        do { \
            DWT->CYCCNT = 0; \
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; \
        } while ();

#define END_COUNT
#define PROFILER_STOP (DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;)


#endif
