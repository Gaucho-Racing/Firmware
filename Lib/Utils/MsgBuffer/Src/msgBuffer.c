#include "msgBuffer.h"

#include <stdbool.h>
#include <stdlib.h>

#define SUCCESS 1
#define FAIL 0

GR_MsgBuffer *GR_MsgBuffer_Create(uint32_t size)
{
	GR_MsgBuffer *msg_buffer = (GR_MsgBuffer *)malloc(sizeof(GR_MsgBuffer));
	msg_buffer->buffer = (uint8_t *)malloc(size * sizeof(uint8_t));
	msg_buffer->max_size = size;
	msg_buffer->free_space = size;
	msg_buffer->head = 0;
	msg_buffer->tail = 0;

	return msg_buffer;
}

void GR_MsgBuffer_Free(GR_MsgBuffer *msg_buffer)
{
	if (msg_buffer) {
		if (msg_buffer->buffer) {
			free(msg_buffer->buffer);
		}
		free(msg_buffer);
	}
}

int8_t GR_MsgBuffer_Push(GR_MsgBuffer *msg_buffer, uint8_t *byte_array, uint8_t size)
{
	if (!msg_buffer || !byte_array) {
		return FAIL;
	}

	if ((uint32_t)(size + 1) > msg_buffer->free_space) {
		return FAIL;
	} else {
		msg_buffer->buffer[msg_buffer->head] = size;
		msg_buffer->head = (msg_buffer->head + 1) % msg_buffer->max_size;

		for (int i = 0; i < size; i++) {
			msg_buffer->buffer[msg_buffer->head] = byte_array[i];
			msg_buffer->head = (msg_buffer->head + 1) % msg_buffer->max_size;
		}
	}

	return SUCCESS;
}

uint32_t GR_MsgBuffer_PeekMsgSize(GR_MsgBuffer *msg_buffer)
{
	if (!msg_buffer) {
		return FAIL;
	}

	if (msg_buffer->free_space == msg_buffer->max_size) {
		return FAIL;
	} else {
		return msg_buffer->buffer[msg_buffer->head];
	}
}

int8_t GR_MsgBuffer_Pop(GR_MsgBuffer *msg_buffer, uint8_t *byte_array)
{
	if (!msg_buffer || !byte_array) {
		return FAIL;
	}

	int msg_size = msg_buffer->buffer[msg_buffer->head];
	msg_buffer->head = (msg_buffer->head + 1) % msg_buffer->max_size;

	if (msg_buffer->free_space == msg_buffer->max_size) {
		return FAIL;
	}

	if (byte_array) {
		for (int i = 0; i < msg_size; i++) {
			byte_array[i] = msg_buffer->buffer[msg_buffer->head];
			msg_buffer->head = (msg_buffer->head + 1) % msg_buffer->max_size;
		}
	}

	msg_buffer->free_space += msg_size + 1;

	return SUCCESS;
}

int8_t GR_MsgBuffer_IsEmpty(GR_MsgBuffer *msg_buffer)
{
	// If the message buffer is NULL, then it's empty (success)
	if (!msg_buffer) {
		return SUCCESS;
	}

	return msg_buffer->free_space == msg_buffer->max_size;
}

uint32_t GR_MsgBuffer_GetCapacity(GR_MsgBuffer *msg_buffer)
{
	if (!msg_buffer) {
		return FAIL;
	}

	if (msg_buffer == 0) {
		return FAIL;
	}
	return msg_buffer->free_space;
}
