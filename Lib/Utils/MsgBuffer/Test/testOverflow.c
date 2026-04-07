#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "msgBuffer.h"

// ARRAY_SIZE must be 1 less than the max number represented by uint8_t
#define ARRAY_SIZE 128

int main(void)
{
	GR_MsgBuffer *buffer_ptr = GR_MsgBuffer_Create(ARRAY_SIZE);
	uint8_t arr1[ARRAY_SIZE], arr2[ARRAY_SIZE];
	int i = 0;

	for (i = 0; i < ARRAY_SIZE; i++) {
		arr1[i] = i;
	}

	for (i = 0; i < 100; i++) {
		GR_MsgBuffer_Push(buffer_ptr, arr1, 2);
	}

	if (GR_MsgBuffer_GetFreeSpace(buffer_ptr) != 2) {
		printf("ERROR: buffer was not filled properly\n");
		return 1;
	}

	while (!GR_MsgBuffer_IsEmpty(buffer_ptr)) {
		if (GR_MsgBuffer_PeekMsgSize(buffer_ptr) != 2) {
			printf("ERROR: buffer message size mismatch\n");
			return 1;
		}
		arr2[0] = ARRAY_SIZE + 1;
		arr2[1] = ARRAY_SIZE + 1;
		GR_MsgBuffer_Pop(buffer_ptr, arr2);
		if (arr2[0] != 0 || arr2[1] != 1) {
			printf("ERROR: buffer array values mismatch\n");
			return 1;
		}
	}

	GR_MsgBuffer_Push(buffer_ptr, arr1, ARRAY_SIZE);
	if (!GR_MsgBuffer_IsEmpty(buffer_ptr)) {
		printf("ERROR: buffer array pushed when it shouldn't have\n");
		return 1;
	}

	GR_MsgBuffer_Push(buffer_ptr, arr1, ARRAY_SIZE - 1);
	if (GR_MsgBuffer_GetFreeSpace(buffer_ptr) != 0) {
		printf("ERROR: buffer array did not push arr1 when it should have\n");
		return 1;
	}

	GR_MsgBuffer_Free(buffer_ptr);

	return 0;
}
