#include "circularBuffer.h"
#include <stdlib.h>

int main()
{

	CircularBuffer* buffer_ptr = NULL;

	// Test for Null error
	GR_CircularBuffer_GetCapacity(buffer_ptr);

	// Test GetCapacity
	buffer_ptr = GR_CircularBuffer_Create(10);
	if (GR_CircularBuffer_GetCapacity(buffer_ptr) != 10) {
		return 1;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	buffer_ptr = GR_CircularBuffer_Create(25);
	if (GR_CircularBuffer_GetCapacity(buffer_ptr) != 25) {
		return 2;
	}
	GR_CircularBuffer_Free(&buffer_ptr);

	return 0;
}
