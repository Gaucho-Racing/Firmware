#include "msgBuffer.h"

#include <stdbool.h>

#define SUCCESS 1
#define FAIL 0

void GR_MsgBuffer_Create(GR_MsgBuffer *msg_buffer, uint32_t size)
{
	msg_buffer->buffer = (uint8_t *)malloc(size * sizeof(uint8_t));
	msg_buffer->max_size = size;
	msg_buffer->free_space = size;
	msg_buffer->head = 0;
	msg_buffer->tail = 0;
}

void GR_MsgBuffer_Free(GR_MsgBuffer *msg_buffer)
{
	free(msg_buffer->buffer);
}

int8_t GR_MsgBuffer_Push(GR_MsgBuffer *msg_buffer, uint8_t *byte_array, uint32_t size)
{
	if ((size + 1) > msg_buffer->free_space) {
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
	if (msg_buffer->free_space == msg_buffer->max_size) {
		return FAIL;
	} else {
		return msg_buffer->buffer[msg_buffer->head];
	}
}

int8_t GR_MsgBuffer_Pop(GR_MsgBuffer *msg_buffer, uint8_t *byte_array)
{
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
	return msg_buffer->free_space == msg_buffer->max_size;
}
