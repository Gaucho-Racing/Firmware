#include <stdio.h>
#include <stdlib.h>

#include "msgBuffer.h"

int main(void)
{

	GR_MsgBuffer *buffer_ptr = NULL;

	buffer_ptr = GR_MsgBuffer_Create(16);

	uint8_t arr1[4];
	arr1[0] = 5;
	arr1[1] = 2;
	arr1[2] = 1;
	arr1[3] = 0;

	GR_MsgBuffer_Push(buffer_ptr, arr1, 4);

	GR_MsgBuffer_Push(buffer_ptr, arr1, 4);

	GR_MsgBuffer_Push(buffer_ptr, arr1, 4);

	if (GR_MsgBuffer_GetFreeSpace(buffer_ptr) != 1) {
		printf("ERROR: free space is not 1\n");
		return 1;
	}

	GR_MsgBuffer_Pop(buffer_ptr, arr1);

	if (GR_MsgBuffer_GetFreeSpace(buffer_ptr) != 6) {
		printf("ERROR: free space is not 6\n");
		return 1;
	}

	GR_MsgBuffer_Pop(buffer_ptr, arr1);

	if (GR_MsgBuffer_GetFreeSpace(buffer_ptr) != 11) {
		printf("ERROR: free space is not 11\n");
		return 1;
	}

	GR_MsgBuffer_Free(buffer_ptr);

	return 0;
}
