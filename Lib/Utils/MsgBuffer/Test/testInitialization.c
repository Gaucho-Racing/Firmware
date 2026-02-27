#include <stdlib.h>
#include <stdio.h>

#include "msgBuffer.h"

int main(void)
{
	GR_MsgBuffer *buffer_ptr;

	// Test for Null error
	buffer_ptr = GR_MsgBuffer_Create(0);
	if (buffer_ptr != NULL) {
		GR_MsgBuffer_Free(buffer_ptr);
		printf("ERROR: create did not work for size 0\n");
		return 1;
	}

	// Basic initialization
	buffer_ptr = GR_MsgBuffer_Create(5);
	if (buffer_ptr == NULL) {
		printf("ERROR: create did not work for size 5\n");
		return 2;
	}

	return 0;
}
