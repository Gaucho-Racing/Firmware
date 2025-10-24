#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer;

	// Is empty
	buffer = GR_CircularBuffer_Create(10);
	if (GR_CircularBuffer_IsFull(buffer)) {
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	// Partially full
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer, &i, size(i));
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		return 2;
	}
	GR_CircularBuffer_Free(buffer);

	// Pushed and popped
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 8; i++) {
		GR_CircularBuffer_Push(buffer, &i, size(i));
	}
	for (int i = 0; i < 5; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		return 3;
	}

	// Full
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 10; i++) {
		GR_CircularBuffer_Push(buffer, &i, size(i));
	}
	if (!GR_CircularBuffer_IsFull(buffer)) {
		return 4;
	}

	// Filled then emptied
	buffer = GR_CircularBuffer_Create(10);
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Push(buffer, &i, size(i));
	}
	for (int i = 0; i < 3; i++) {
		GR_CircularBuffer_Pop(buffer);
	}
	if (GR_CircularBuffer_IsFull(buffer)) {
		return 5;
	}

	return 0;
}