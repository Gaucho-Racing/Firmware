#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer;

	// Test GetCurrentSize
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	// Test single item
	int temp = 0;
	GR_CircularBuffer_Push(buffer, &temp);
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 1) {
		GR_CircularBuffer_Free(buffer);
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	GR_CircularBuffer_Push(buffer, &temp);
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 1) {
		GR_CircularBuffer_Free(buffer);
		return 2;
	}
	GR_CircularBuffer_Free(buffer);

	// Push multiple items
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr1[6] = {0};
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer, &arr1[i]);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		GR_CircularBuffer_Free(buffer);
		return 3;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer, &arr1[i]);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		GR_CircularBuffer_Free(buffer);
		return 4;
	}
	GR_CircularBuffer_Free(buffer);

	// Push over limit
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	int arr2[12] = {0};
	for (int i = 0; i < 12; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 10) {
		GR_CircularBuffer_Free(buffer);
		return 5;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	for (int i = 0; i < 12; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 10) {
		GR_CircularBuffer_Free(buffer);
		return 6;
	}
	GR_CircularBuffer_Free(buffer);

	// Push and pop
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	for (int i = 0; i < 9; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 5) {
		GR_CircularBuffer_Free(buffer);
		return 7;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	for (int i = 0; i < 9; i++) {
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 5) {
		GR_CircularBuffer_Free(buffer);
		return 8;
	}
	GR_CircularBuffer_Free(buffer);

	// Push over limit and pop
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr3[15] = {0};
	for (int i = 0; i < 15; i++) {
		GR_CircularBuffer_Push(buffer, &arr3[i]);
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		GR_CircularBuffer_Free(buffer);
		return 9;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	int arr4[15] = {0};
	for (int i = 0; i < 15; i++) {
		GR_CircularBuffer_Push(buffer, &arr4[i]);
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		GR_CircularBuffer_Free(buffer);
		return 10;
	}
	GR_CircularBuffer_Free(buffer);

	return 0;
}
