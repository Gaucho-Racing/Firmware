#include <stdlib.h>

#include "msgBuffer.h"

int main(void)
{

	GR_MsgBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (GR_MsgBuffer_GetCapacity(buffer_ptr) != 0) {
		return 1;
	}

	// Test GetCapacity
	buffer_ptr = GR_MsgBuffer_Create(10);
	if (GR_MsgBuffer_GetCapacity(buffer_ptr) != 10) {
		return 2;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	buffer_ptr = GR_MsgBuffer_Create(25);
	if (GR_MsgBuffer_GetCapacity(buffer_ptr) != 25) {
		return 3;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	return 0;
}
