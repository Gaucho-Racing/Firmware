#include <stdlib.h>

#include "circularBuffer.h"

int main(void)
{
	CircularBuffer *buffer_ptr;

	// Test for Null error
	buffer_ptr = GR_CircularBuffer_Create(0);
	if (buffer_ptr != NULL) {
		GR_CircularBuffer_Free(&buffer_ptr);
		return 1;
	}

	// Basic initialization
	buffer_ptr = GR_CircularBuffer_Create(5);
	if (buffer_ptr == NULL) {
		return 2;
	}
	GR_CircularBuffer_Free(&buffer_ptr);
	// De-allocator should nullify the buffer's pointer.
	if (buffer_ptr != NULL) {
		return 3;
	}

	return 0;
}
