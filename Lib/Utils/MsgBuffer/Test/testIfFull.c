#include <stdlib.h>

#include "msgBuffer.h"

int main(void)
{

	CircularBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (GR_MsgBuffer_IsFull(buffer_ptr)) {
		return 1;
	}

	// Is empty
	buffer_ptr = GR_MsgBuffer_Create(10);
	if (GR_MsgBuffer_IsFull(buffer_ptr)) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 2;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Partially full
	buffer_ptr = GR_MsgBuffer_Create(10);
	int arr1[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr1[i], sizeof(arr1[i]));
	}
	if (GR_MsgBuffer_IsFull(buffer_ptr)) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 3;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Pushed and popped
	buffer_ptr = GR_MsgBuffer_Create(10);
	int arr2[8] = {0};
	for (int i = 0; i < 8; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr2[i], sizeof(arr2[i]));
	}
	for (int i = 0; i < 5; i++) {
		free(GR_MsgBuffer_Pop(buffer_ptr));
	}
	if (GR_MsgBuffer_IsFull(buffer_ptr)) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 4;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Full
	buffer_ptr = GR_MsgBuffer_Create(10);
	int arr3[10] = {0};
	for (int i = 0; i < 10; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr3[i], sizeof(arr3[i]));
	}
	if (!GR_MsgBuffer_IsFull(buffer_ptr)) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 5;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Filled then emptied
	buffer_ptr = GR_MsgBuffer_Create(10);
	int arr4[3] = {0};
	for (int i = 0; i < 3; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr4[i], sizeof(arr4[i]));
	}
	for (int i = 0; i < 3; i++) {
		free(GR_MsgBuffer_Pop(buffer_ptr));
	}
	if (GR_MsgBuffer_IsFull(buffer_ptr)) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 6;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	return 0;
}
