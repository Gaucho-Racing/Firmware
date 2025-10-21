#include "circularBuffer.h"

int main()
{
	CircularBuffer *buffer = GR_CircularBuffer_Create(10);

	// pushing without overwriting
	for (int i = 0; i < 10; i++) {
		int tmp = i;
		GR_CircularBuffer_Push(buffer, &tmp, sizeof(tmp));
	}
	for (int i = 0; i < 1000; i++) {
		int *tmp;
		tmp = GR_CircularBuffer_Peek(buffer);
		if (*tmp != 0) { // should not modify the buffer
			GR_CircularBuffer_Free(buffer);
			return 1;
		}
	}

	GR_CircularBuffer_Free(buffer);
	return 0;
}
