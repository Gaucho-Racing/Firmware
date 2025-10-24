#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer;

	// Test GetCurrentSize
	buffer = GR_CircularBuffer_Create(10);
	// Test single item
	int temp = 0;
	GR_CircularBuffer_Push(buffer, &temp, sizeof(temp));
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 1) {
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	// Push multiple items
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 6; i++) {
		GR_CircularBuffer_Push(buffer, &i, sizeof(temp));
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		return 2;
	}
	GR_CircularBuffer_Free(buffer);

	// Push over limit
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 12; i++) {
		GR_CircularBuffer_Push(buffer, &i, sizeof(temp));
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 10) {
		return 3;
	}
	GR_CircularBuffer_Free(buffer);

	// Push and pop
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 9; i++) {
		GR_CircularBuffer_Push(buffer, &i, sizeof(temp));
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 5) {
		return 4;
	}
	GR_CircularBuffer_Free(buffer);

	// Push over limit and pop
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 15; i++) {
		GR_CircularBuffer_Push(buffer, &i, sizeof(temp));
	}
	for (int i = 0; i < 4; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_GetCurrentSize(buffer) != 6) {
		return 5;
	}
	GR_CircularBuffer_Free(buffer);

	return 0;
}