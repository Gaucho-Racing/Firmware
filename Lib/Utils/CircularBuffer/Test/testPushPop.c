#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer = GR_CircularBuffer_Create(10);

	// pushing without overwriting
	for (int i = 0; i < 10; i++) {
		int tmp = i;
		GR_CircularBuffer_Push(buffer, &tmp, sizeof(tmp));
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer);
		if (*tmp != i) {
			GR_CircularBuffer_Free(buffer);
			return 1;
		}
	}

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

	// pushing past size limite
	for (int i = 0; i < 1000; i++) {
		int tmp = i;
		GR_CircularBuffer_Push(buffer, &tmp, sizeof(tmp));
	}
	for (int i = 0; i < 10; i++) {
		int tmp = i;
		GR_CircularBuffer_Push(buffer, &tmp, sizeof(tmp));
	}
	for (int i = 0; i < 10; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Pop(buffer);
		if (*tmp != i) {
			GR_CircularBuffer_Free(buffer);
			return 3;
		}
	}

	GR_CircularBuffer_Free(buffer);
	return 0;
}
