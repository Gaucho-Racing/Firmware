#ifndef PROFILE_H
#define PROFILE_H

#define PROFILE 1

#include <stdbool.h>
#include <stdint.h>

#include "Logomatic.h"

static inline uint32_t max_u32(uint32_t a, uint32_t b)
{
	return a > b ? a : b;
}
typedef struct dwt_timer_t {
	uint32_t total_cycles;
	uint32_t total_samples;

	volatile uint32_t start_cycle;
	volatile uint32_t end_cycle;

	volatile uint32_t max_cycles;
	volatile uint32_t min_cycles;

	volatile bool measuring;
} dwt_timer_t;

static inline void start_dwt()
{
	// DWT->CYCCNT = 0;
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	__ISB();
}

static inline void stop_dwt()
{
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

// static number of cycles is 37
#define STATIC_CYCLE_COST 36

static inline float dwt_timer_average_cycles(const dwt_timer_t *dt)
{
	return (dt->total_cycles) / ((float)dt->total_samples);
}
static inline void dwt_timer_start_measurement(dwt_timer_t *dt)
{
	dt->measuring = true;
	dt->start_cycle = DWT->CYCCNT;
	__DSB();
	__ISB();
}



static inline void dwt_timer_end_measurement(dwt_timer_t *dt)
{
	if (!dt->measuring) {
		return;
	}

	dt->end_cycle = DWT->CYCCNT;

	// handles one overflow
	uint32_t elapsed_cycle = dt->end_cycle - dt->start_cycle;

	dt->total_cycles += elapsed_cycle; //- STATIC_CYCLE_COST;
	dt->total_samples++;

	dt->measuring = false;

	dt->max_cycles = max_u32(dt->max_cycles, elapsed_cycle);
	__DSB();
}

static inline void dwt_timer_reset(dwt_timer_t *dt)
{
	dwt_timer_end_measurement(dt);

	dt->start_cycle = 0;
	dt->end_cycle = 0;
	dt->measuring = false;
	dt->total_cycles = 0;
	dt->total_samples = 0;
}

static inline void dwt_timer_print_info(dwt_timer_t *dt)
{
	LOGOMATIC("AVG: %f\n", dwt_timer_average_cycles(dt));
	LOGOMATIC("Total Cycles: %lu\n", dt->total_cycles);
	LOGOMATIC("Total Samples: %lu\n", dt->total_samples);
	LOGOMATIC("Max Cycle: %lu\n", dt->max_cycles);
}

// testing
extern void profiler_test();

/*extern dwt_timer_t GLOBAL_DWT_TIMER;

//static number of cycles is 30
static inline void global_dwt_timer_start_measurement(void) {
    GLOBAL_DWT_TIMER.measuring = true;
    GLOBAL_DWT_TIMER.start_cycle = DWT->CYCCNT;
}

static inline void global_dwt_timer_end_measurement(void) {
    if (!GLOBAL_DWT_TIMER.measuring) {
	return;
    }

    GLOBAL_DWT_TIMER.end_cycle = DWT->CYCCNT;

    //handles one overflow
    uint32_t elapsed_cycle = GLOBAL_DWT_TIMER.end_cycle - GLOBAL_DWT_TIMER.start_cycle;
    GLOBAL_DWT_TIMER.total_cycles += elapsed_cycle;
    GLOBAL_DWT_TIMER.total_samples++;

    GLOBAL_DWT_TIMER.measuring = false;

    GLOBAL_DWT_TIMER.max_cycles = MAX(GLOBAL_DWT_TIMER.max_cycles, elapsed_cycle);
    //GLOBAL_DWT_TIMER.min_cycles;
}
static inline float global_dwt_timer_average_cycles() {
    return (GLOBAL_DWT_TIMER.total_cycles)/((float) GLOBAL_DWT_TIMER.total_samples);

}*/

// extern volatile uint32_t PROFILE_AVG_RX_CYCLES;
// extern volatile uint32_t PROFILE_RX_SAMPLES;
// extern volatile uint32_t ELAPSED_CYCLES;
// extern volatile float AVG;

/*#define START_COUNT \
	do { \
	    DWT->CYCCNT = 0; \
	    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; \
	} while ();

#define END_COUNT
#define PROFILER_STOP (DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;)
*/

#endif
