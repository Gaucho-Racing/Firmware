#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "msgBuffer.h"

int main(void)
{

	GR_MsgBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (!GR_MsgBuffer_IsEmpty(buffer_ptr)) {
		printf("ERROR: null test failed\n");
		return 1;
	}

	buffer_ptr = GR_MsgBuffer_Create(3);

	uint8_t arr1[2];
	arr1[0] = 5;
	arr1[1] = 2;

	GR_MsgBuffer_Push(buffer_ptr, arr1, 2);

	if (GR_MsgBuffer_IsEmpty(buffer_ptr)) {
		printf("ERROR: buffer is still empty\n");
		return 1;
	}

	return 0;
}
