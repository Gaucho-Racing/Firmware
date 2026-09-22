#include "CriticalSection.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include "Unused.h"

#define NUM_THREADS 40
#define INCREMENTS_PER_THREAD 100000

pthread_mutex_t __mock_global_irq_mutex;
_Thread_local uint32_t __mock_primask_state = 0;
_Thread_local uint32_t __mock_irq_nesting_depth = 0;

__attribute__((constructor)) static void init_recursive_mock_mutex(void)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&__mock_global_irq_mutex, &attr);
	pthread_mutexattr_destroy(&attr);
}

volatile long long global_counter = 0;

void *thread_worker_good(void *arg)
{
	UNUSED(arg);

	for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
		CRITICAL_SECTION
		{
			CRITICAL_SECTION
			{
				global_counter++;
			}
		}
	}

	return NULL;
}

void *thread_worker_bad(void *arg)
{
	UNUSED(arg);

	for (int i = 0; i < INCREMENTS_PER_THREAD; i++) {
		global_counter++;
	}

	return NULL;
}

int main(void)
{
	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];

	printf("Starting %d 'good' threads, each incrementing %d times...\n", NUM_THREADS, INCREMENTS_PER_THREAD);
	global_counter = 0;

	for (int i = 0; i < NUM_THREADS; i++) {
		thread_ids[i] = i;
		if (pthread_create(&threads[i], NULL, thread_worker_good, &thread_ids[i]) != 0) {
			printf("Failed to create thread");
			return 1;
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	long long expected_value = (long long)NUM_THREADS * INCREMENTS_PER_THREAD;
	printf("\tExpected final counter value: %lld\n", expected_value);
	printf("\tActual global_counter value:  %lld\n", global_counter);

	if (global_counter == expected_value) {
		printf("\tMocks and nested critical sections passed!\n");
	} else {
		printf("\tData race detected! Your critical section failed.\n");
		return 2;
	}

	printf("\nStarting %d 'bad' threads, each incrementing %d times...\n", NUM_THREADS, INCREMENTS_PER_THREAD);
	global_counter = 0;

	for (int i = 0; i < NUM_THREADS; i++) {
		thread_ids[i] = i;
		if (pthread_create(&threads[i], NULL, thread_worker_bad, &thread_ids[i]) != 0) {
			printf("Failed to create thread");
			return 3;
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	printf("\tExpected final counter value: %lld\n", expected_value);
	printf("\tActual global_counter value:  %lld\n", global_counter);
	if (global_counter == expected_value) {
		printf("\tUnexpectedly, the counter is correct without critical sections. This is highly unlikely and may indicate a problem with the test setup.\n");
		return 4;
	}

	printf("\tAs expected, the counter is incorrect without critical sections. Data race detected!\n");
	return 0;
}
