#ifndef MSG_BUFFER_H
#define MSG_BUFFER_H

#include <stdint.h>

typedef struct msg_buffer_stc {
	uint8_t *buffer;
	uint32_t head;
	uint32_t tail;
	uint32_t free_space;
	uint32_t max_size;
} GR_MsgBuffer;

GR_MsgBuffer* GR_MsgBuffer_Create(uint32_t size);

void GR_MsgBuffer_Free(GR_MsgBuffer *msg_buffer);

// Returns 0 on FAIL and 1 on SUCCESS
int8_t GR_MsgBuffer_Push(GR_MsgBuffer *msg_buffer, uint8_t *byte_array, uint8_t size);

// Returns 0 on FAIL and size on SUCCESS
uint32_t GR_MsgBuffer_PeekMsgSize(GR_MsgBuffer *msg_buffer);

// Returns 0 on FAIL and 1 on SUCCESS
int8_t GR_MsgBuffer_Pop(GR_MsgBuffer *msg_buffer, uint8_t *byte_array);

// Returns 0 on FALSE and 1 on TRUE
int8_t GR_MsgBuffer_IsEmpty(GR_MsgBuffer *msg_buffer);

// Returns the remaining capacity (0 on null msg_buffer)
int GR_MsgBuffer_GetCapacity(GR_MsgBuffer *msg_buffer);

#endif // MSG_BUFFER_H
