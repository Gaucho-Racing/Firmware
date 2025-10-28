#include "circularBuffer.h"
#include "stdlib.h"

int main()
{

	CircularBuffer *buffer;

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr1[6] = {1, 2, 3, 4, 5, 6};
	// pushing without overwriting
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer, &arr1[i]);
	}
	for (int i = 0; i < 6; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer);
		if (*tmp != arr1[i]) {
			GR_CircularBuffer_Free(buffer);
			return 1;
		}
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    buffer, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	// poping empty buffer
	for (int i = 0; i < 1000; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	for (int i = 0; i < 1000; i++) {
		if (GR_CircularBuffer_Pop(buffer) !=
		    NULL) { // Should do nothing
			GR_CircularBuffer_Free(buffer);
			return 2;
		}
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	int arr2[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	// pushing past size limit
	for (int i = 0; i < 13; i++) {
		int tmp = i;
		GR_CircularBuffer_Push(buffer, &arr2[i]);
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer);
		if (*tmp != arr2[i + 3]) {
			GR_CircularBuffer_Free(buffer);
			return 3;
		}
	}
	GR_CircularBuffer_Free(buffer);

	// Edge case: Buffer with capacity 1
	CircularBuffer *bufferOne =
	    GR_CircularBuffer_Create(1, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);

	// Pushing and popping one element
	int num = 0;
	GR_CircularBuffer_Push(bufferOne, &num);

	int *tmp = GR_CircularBuffer_Pop(bufferOne);
	if (*tmp != num) {
		GR_CircularBuffer_Free(bufferOne);
		return 4;
	}

	// Pushing beyond limit (same element should repeatedly get overwritten)
	int arr3[2] = {1, 2};
	for (int i = 0; i < 2; i++) {
		GR_CircularBuffer_Push(bufferOne, &arr3[i]);
	}

	tmp = GR_CircularBuffer_Pop(
	    bufferOne); // Should contain the last pushed element, 1
	if (*tmp != 2) {
		return 5;
	}
	GR_CircularBuffer_Free(bufferOne);

	return 0;
}
