#include <stdlib.h>

#include "msgBuffer.h"

int main(void)
{
	GR_MsgBuffer *buffer_ptr = NULL;

	// Test for Null error
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 0) {
		return 1;
	}

	// Test GetCurrentSize
	GR_MsgBuffer *buffer_ptr = (GR_MsgBuffer *)malloc(sizeof(GR_MsgBuffer));
	GR_MsgBuffer_Create(buffer_ptr, sizeof(GR_MsgBuffer));

	// Test single item
	uint8_t temp[3] = {1};
	GR_MsgBuffer_Push(buffer_ptr, &temp, sizeof(temp));
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 1) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 2;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	GR_MsgBuffer_Create(10);
	GR_MsgBuffer_Push(buffer_ptr, &temp, sizeof(temp));
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 1) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 3;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Push multiple items
	GR_MsgBuffer_Create(10);
	int arr1[6] = {0};
	for (int i = 0; i < 6; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr1[i], sizeof(arr1[i]));
	}
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 6) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 4;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Push over limit
	GR_MsgBuffer_Create(10);
	int arr2[12] = {0};
	for (int i = 0; i < 12; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr2[i], sizeof(arr2[i]));
	}
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 10) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 5;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Push and pop
	buffer_ptr = GR_MsgBuffer_Create(10);
	for (int i = 0; i < 9; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr2[i], sizeof(arr2[i]));
	}
	for (int i = 0; i < 4; i++) {
		free(GR_MsgBuffer_Pop(buffer_ptr));
	}
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 5) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 6;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	// Push over limit and pop
	GR_MsgBuffer_Create(10);
	int arr4[15] = {0};
	for (int i = 0; i < 15; i++) {
		GR_MsgBuffer_Push(buffer_ptr, &arr4[i], sizeof(arr4[i]));
	}
	for (int i = 0; i < 4; i++) {
		free(GR_MsgBuffer_Pop(buffer_ptr));
	}
	if (GR_MsgBuffer_GetCurrentSize(buffer_ptr) != 6) {
		GR_MsgBuffer_Free(&buffer_ptr);
		return 7;
	}
	GR_MsgBuffer_Free(&buffer_ptr);

	return 0;
}
