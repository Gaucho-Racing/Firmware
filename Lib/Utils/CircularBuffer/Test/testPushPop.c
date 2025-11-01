#include "circularBuffer.h"
#include <stdlib.h>

int main()
{

	CircularBuffer *buffer_ptr = NULL;

	// Test for Null error when pushing into null buffer
	int temp = 1;
	if (GR_CircularBuffer_Push(buffer_ptr, &temp, sizeof(temp)) != 1) {
		return 1;
	}

	// Test for Null error when popping from null buffer
	{
		int *ptr = GR_CircularBuffer_Pop(buffer_ptr);
		if (ptr != NULL) {
			free(ptr);
			return 2;
		}
	}

	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr1[6] = {1, 2, 3, 4, 5, 6};
	// pushing without overwriting
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr1[i], sizeof(arr1[i]));
	}
	for (int i = 0; i < 6; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer_ptr);
		if (*tmp != arr1[i]) {
			free(tmp);
			GR_CircularBuffer_Free(&buffer_ptr);
			return 3;
		}
		free(tmp);
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	buffer_ptr = GR_CircularBuffer_Create(10);
	// popping empty buffer
	for (int i = 0; i < 1000; i++) {
		free(GR_CircularBuffer_Pop(buffer_ptr));
	}
	for (int i = 0; i < 1000; i++) {
		int *ptr = GR_CircularBuffer_Pop(buffer_ptr);
		if (ptr != NULL) { // Should do nothing
			free(ptr);
			GR_CircularBuffer_Free(&buffer_ptr);
			return 4;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// pushing past size limit
	buffer_ptr = GR_CircularBuffer_Create(20);
	int arr2[1000];
	for (int i = 0; i < 1000; i++) {
		arr2[i] = i;
	}
	for (int i = 0; i < 1000; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr2[i], sizeof(arr2[i]));
	}
	for (int i = 0; i < 20; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer_ptr);
		if (*tmp != arr2[i + 1000 - 20]) {
			free(tmp);
			GR_CircularBuffer_Free(&buffer_ptr);
			return 5;
		}
		free(tmp);
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Edge case: Buffer with capacity 1
	buffer_ptr = GR_CircularBuffer_Create(1);

	// Pushing and popping one element
	int num = 0;
	GR_CircularBuffer_Push(buffer_ptr, &num, sizeof(num));

	int *tmp = GR_CircularBuffer_Pop(buffer_ptr);
	if (*tmp != num) {
		free(tmp);
		GR_CircularBuffer_Free(&buffer_ptr);
		return 6;
	}
	free(tmp);

	// Pushing beyond limit (same element should repeatedly get overwritten)
	int arr3[2] = {1, 2};
	for (int i = 0; i < 2; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr3[i], sizeof(arr3[i]));
	}

	tmp = GR_CircularBuffer_Pop(
	    buffer_ptr); // Should contain the last pushed element, 1
	if (*tmp != 2) {
		free(tmp);
		return 7;
	}
	free(tmp);

	// Pushing limited-scoped variable to the buffer (should persist in the
	// buffer)
	{
		int local = 114514;
		GR_CircularBuffer_Push(buffer_ptr, &local, sizeof(local));
	}
	{
		int *ptr = GR_CircularBuffer_Pop(buffer_ptr);
		if (*ptr != 114514) {
			free(ptr);
			return 8;
		}
		free(ptr);
		GR_CircularBuffer_Free(&buffer_ptr);
	}

	// Stress Test push and free
	buffer_ptr = GR_CircularBuffer_Create(100);
	for (int i = 0; i < 1000; i++) {
		if (GR_CircularBuffer_Push(buffer_ptr, &i, sizeof(i))) {
			return 9;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	return 0;
}
