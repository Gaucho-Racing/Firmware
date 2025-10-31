#include "circularBuffer.h"
#include <stdlib.h>

int main()
{

	CircularBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		return 1;
	}

	// Is empty
	buffer_ptr = GR_CircularBuffer_Create(10);
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 2;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Partially full
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr1[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr1[i]);
	}
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 3;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Pushed and popped
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr2[8] = {0};
	for (int i = 0; i < 8; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr2[i]);
	}
	for (int i = 0; i < 5; i++) {
		GR_CircularBuffer_Pop(buffer_ptr);
	}
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 4;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Full
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr3[10] = {0};
	for (int i = 0; i < 10; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr3[i]);
	}
	if (!GR_CircularBuffer_IsFull(buffer_ptr)) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 5;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Filled then emptied
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr4[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr4[i]);
	}
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Pop(buffer_ptr);
	}
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 6;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	return 0;
}
