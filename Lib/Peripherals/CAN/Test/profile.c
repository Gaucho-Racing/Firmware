#include "profile.h"

dwt_timer_t GLOBAL_DWT_TIMER = {0};


void profiler_test() {
    dwt_timer_t timer = {0};

    start_dwt();

    for (int i = 0; i < 100; i ++) {
		//global_dwt_timer_start_measurement();
		dwt_timer_start_measurement(&timer);
		//HAL_Delay(1);
		//for (int i = 0; i < 10; i++);
		dwt_timer_end_measurement(&timer);
		//global_dwt_timer_start_measurement();
	}

    dwt_timer_print_info(&timer);

    stop_dwt();
}
