#include "circularBuffer.h"
#include <stdlib.h>

int main()
{
	CircularBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (GR_CircularBuffer_Peek(buffer_ptr) != NULL) {
		return 1;
	}

	// Empty buffer
	buffer_ptr = GR_CircularBuffer_Create(10);
	void *ptr1 = GR_CircularBuffer_Peek(buffer_ptr);
	if (ptr1 != NULL) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 2;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Pushing without overwriting
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr1[6] = {1, 2, 3, 4, 5, 6};
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr1[i], sizeof(arr1[i]));
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer_ptr);
		if (*tmp != 1) { // should not modify the buffer
			GR_CircularBuffer_Free(&buffer_ptr);
			return 3;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Pushing with overwriting
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr2[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	for (int i = 0; i < 13; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr2[i], sizeof(arr2[i]));
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer_ptr);
		if (*tmp != 4) { // should not modify the buffer
			GR_CircularBuffer_Free(&buffer_ptr);
			return 4;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	// Pushing and popping
	buffer_ptr = GR_CircularBuffer_Create(10);
	int arr3[7] = {1, 2, 3, 4, 5, 6, 7};
	for (int i = 0; i < 7; i++) {
		GR_CircularBuffer_Push(buffer_ptr, &arr3[i], sizeof(arr3[i]));
	}
	for (int i = 0; i < 3; i++) {
		free(GR_CircularBuffer_Pop(buffer_ptr));
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer_ptr);
		if (*tmp != 4) { // should not modify the buffer
			GR_CircularBuffer_Free(&buffer_ptr);
			return 5;
		}
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	return 0;
}
