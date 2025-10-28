#include "circularBuffer.h"

int main()
{

	CircularBuffer *buffer;

	// Test GetCapacity
	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	if (GR_CircularBuffer_GetCapacity(buffer) != 10) {
		return 1;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    10, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	if (GR_CircularBuffer_GetCapacity(buffer) != 10) {
		return 2;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    25, CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE);
	if (GR_CircularBuffer_GetCapacity(buffer) != 25) {
		return 3;
	}
	GR_CircularBuffer_Free(buffer);

	buffer = GR_CircularBuffer_Create(
	    25, CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE);
	if (GR_CircularBuffer_GetCapacity(buffer) != 25) {
		return 4;
	}
	GR_CircularBuffer_Free(buffer);

	return 0;
}
