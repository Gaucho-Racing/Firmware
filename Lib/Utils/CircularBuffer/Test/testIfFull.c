#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer;

	// Is empty
	buffer = GR_CircularBuffer_Create(10);
	if (GR_CircularBuffer_IsFull(buffer)) {
		GR_CircularBuffer_Free(buffer);
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	// Partially full
	buffer = GR_CircularBuffer_Create(10);
	int arr1[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer, &arr1[i]);
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		GR_CircularBuffer_Free(buffer);
		return 2;
	}
	GR_CircularBuffer_Free(buffer);

	// Pushed and popped
	buffer = GR_CircularBuffer_Create(10);
	int arr2[8] = {0};
	for (int i = 0; i < 8; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	for (int i = 0; i < 5; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		GR_CircularBuffer_Free(buffer);
		return 3;
	}
	GR_CircularBuffer_Free(buffer);

	// Full
	buffer = GR_CircularBuffer_Create(10);
	int arr3[10] = {0};
	for (int i = 0; i < 10; i++) {
		GR_CircularBuffer_Push(buffer, &arr3[i]);
	}
	if (!GR_CircularBuffer_IsFull(buffer)) {
		GR_CircularBuffer_Free(buffer);
		return 4;
	}
	GR_CircularBuffer_Free(buffer);

	// Filled then emptied
	buffer = GR_CircularBuffer_Create(10);
	int arr4[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer, &arr4[i]);
	}
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		GR_CircularBuffer_Free(buffer);
		return 5;
	}
	GR_CircularBuffer_Free(buffer);

	return 0;
}
