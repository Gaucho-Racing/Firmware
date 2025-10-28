#include "circularBuffer.h"
#include <stdlib.h>

int main()
{
	CircularBuffer *buffer;

	// Empty buffer
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	void *ptr1 = GR_CircularBuffer_Peek(buffer);
	if (ptr1 != NULL) {
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	// Pushing without overwriting
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr1[6] = {1, 2, 3, 4, 5, 6};
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer, &arr1[i]);
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer);
		if (*tmp != 1) { // should not modify the buffer
			return 2;
		}
	}
	GR_CircularBuffer_Free(buffer);

	// Pushing with overwriting
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr2[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	for (int i = 0; i < 13; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer);
		if (*tmp != 4) { // should not modify the buffer
			return 3;
		}
	}
	GR_CircularBuffer_Free(buffer);

	// Pushing and popping
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr3[7] = {1, 2, 3, 4, 5, 6, 7};
	for (int i = 0; i < 7; i++) {
		GR_CircularBuffer_Push(buffer, &arr3[i]);
	}
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer);
		if (*tmp != 4) { // should not modify the buffer
			return 4;
		}
	}
	GR_CircularBuffer_Free(buffer);

	return 0;
}
