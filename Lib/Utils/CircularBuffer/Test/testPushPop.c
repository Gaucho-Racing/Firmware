#include "circularBuffer.h"
#include <stdlib.h>

int main()
{

	CircularBuffer *buffer_ptr = NULL;

	// Test for Null error when pushing into null buffer
	int temp = 1;
	if (GR_CircularBuffer_Push(buffer_ptr, &temp) != 1) {
		return 1;
	}

	// Test for Null error when popping from null buffer
	if (GR_CircularBuffer_Pop(buffer_ptr) != NULL) {
		return 2;
	}

	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr1[6] = {1, 2, 3, 4, 5, 6};
	// pushing without overwriting
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr1[i]);
	}
	for (int i = 0; i < 6; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer_ptr);
		if (*tmp != arr1[i]) {
			GR_CircularBuffer_Free(&buffer_ptr);
			return 3;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	buffer_ptr = GR_CircularBuffer_Create(10);
	// popping empty buffer
	for (int i = 0; i < 1000; i++) {
		GR_CircularBuffer_Pop(buffer_ptr);
	}
	for (int i = 0; i < 1000; i++) {
		if (GR_CircularBuffer_Pop(buffer_ptr) !=
		    NULL) { // Should do nothing
			GR_CircularBuffer_Free(&buffer_ptr);
			return 4;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr2[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	// pushing past size limit
	for (int i = 0; i < 13; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr2[i]);
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer_ptr);
		if (*tmp != arr2[i + 3]) {
			GR_CircularBuffer_Free(&buffer_ptr);
			return 5;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Edge case: Buffer with capacity 1
	buffer_ptr = GR_CircularBuffer_Create(1);

	// Pushing and popping one element
	int num = 0;
	GR_CircularBuffer_Push(buffer_ptr, &num);

	int *tmp = GR_CircularBuffer_Pop(buffer_ptr);
	if (*tmp != num) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 6;
	}

	// Pushing beyond limit (same element should repeatedly get overwritten)
	int arr3[2] = {1, 2};
	for (int i = 0; i < 2; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr3[i]);
	}

	tmp = GR_CircularBuffer_Pop(
	    buffer_ptr); // Should contain the last pushed element, 1
	if (*tmp != 2) {
		return 7;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	return 0;
}
