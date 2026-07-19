#include <stdint.h>

#include "Stringification.h"

#ifndef CRITICAL_SECTION_H
#define CRITICAL_SECTION_H

#ifdef HOOTL_TEST
#include <pthread.h>

extern pthread_mutex_t __mock_global_irq_mutex;
extern uint32_t __mock_primask_state;

static inline uint32_t __get_PRIMASK(void)
{
	return __mock_primask_state;
}

static inline void __disable_irq(void)
{
	pthread_mutex_lock(&__mock_global_irq_mutex);
	__mock_primask_state = 1;
}

static inline void __set_PRIMASK(uint32_t state)
{
	pthread_mutex_unlock(&__mock_global_irq_mutex);
	__mock_primask_state = state;
}
#endif

/**
 * @brief Internal function to automatically restore the interrupt state when exiting a critical section.
 *
 * This function is intended to be used with the GCC cleanup attribute to automatically restore the interrupt state when a critical section is exited. It takes a pointer to a state variable that holds
 * the previous interrupt state and restores it using the __set_PRIMASK function.
 *
 * @warning This function is intended for internal use only and should not be called directly by user code. Use the CRITICAL_SECTION macro instead.
 */
static inline void _magic_auto_critical_exit(uint32_t *state_var)
{
	if (state_var) {
		__set_PRIMASK(*state_var);
	}
}

/**
 * @brief Macro to create a critical section that disables interrupts for the duration of the block.
 *
 * This macro uses the GCC cleanup attribute to automatically restore the interrupt state when the block is exited, regardless of how the block is exited (e.g., return, break, continue).
 *
 * @warning This macro should be used with caution, as it can lead to deadlocks or other issues if not used correctly. It is recommended to use this macro only in situations where it is necessary to
 * disable interrupts for a short period of time.
 */
#define CRITICAL_SECTION                                                                                                                                                                               \
	for (__attribute__((cleanup(_magic_auto_critical_exit))) uint32_t CONCAT(_magic_auto_critical_state_, __LINE__) = __get_PRIMASK(), CONCAT(_magic_auto_critical_run_, __LINE__) = (__disable_irq(), 1);                         \
	     CONCAT(_magic_auto_critical_run_, __LINE__); CONCAT(_magic_auto_critical_run_, __LINE__) = 0)

#endif
