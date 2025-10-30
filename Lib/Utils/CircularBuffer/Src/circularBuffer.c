#include "circularBuffer.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Define the circularBuffer data type
struct circular_buffer_st {
	uint16_t head;	   // the head position inclusive
	uint16_t tail;	   // the tail position exclusive
	uint16_t capacity; // the buffer's capacity in number of items
	void **buffer;	   // the buffer body
};

CircularBuffer_Ptr GR_CircularBuffer_Create(uint16_t capacity)
{
	// Return null pointer if an invalid size(< 1) is specified
	if (capacity < 1) {
		return NULL;
	}
	// For specifications see the header file
	CircularBuffer_Ptr buffer = malloc(sizeof(CircularBuffer));
	buffer->head = 0;
	buffer->tail = 0;
	buffer->capacity = capacity;
	buffer->buffer = calloc(capacity, sizeof(void *));
	return buffer;
}

void GR_CircularBuffer_Free(CircularBuffer_Ptr *bufferPtr)
{
	// Error check
	if (*bufferPtr == NULL) {
		return;
	}
	// Free buffer body
	free((*bufferPtr)->buffer);
	// Free buffer instance
	free(*bufferPtr);

	*bufferPtr = NULL;
}

uint16_t GR_CircularBuffer_GetCapacity(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (buffer == NULL) {
		return -1;
	}

	return buffer->capacity;
}

uint16_t GR_CircularBuffer_GetCurrentSize(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (buffer == NULL) {
		return -1;
	}
	// Empty case, since occupied member must have a non-null wrapper
	// pointer
	if (buffer->buffer[buffer->head] == NULL) {
		return 0;
	}
	// Account for circular buffer's loopback behaviour
	if (buffer->tail <= buffer->head) {
		return buffer->tail + buffer->capacity - buffer->head;
	}
	// Normal case
	return buffer->tail - buffer->head;
}

bool GR_CircularBuffer_IsFull(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (!buffer) {
		return true;
	}
	// In the case where head and tail iterators are equal,
	// the buffer is full if any entry in the buffer is occupied.
	return buffer->head == buffer->tail && buffer->buffer[0] != NULL;
}

bool GR_CircularBuffer_IsEmpty(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (!buffer) {
		return true;
	}
	// In the case where head and tail iterators are equal,
	// the buffer is empty if any entry in the buffer is free.
	return buffer->head == buffer->tail && buffer->buffer[0] == NULL;
}

int GR_CircularBuffer_Push(CircularBuffer_Ptr buffer, void *object)
{
	// Buffer null error
	if (!buffer) {
		return -1;
	}
	// Remove the buffer head if it's going to be overwritten
	// That is, if the buffer is already full
	if (GR_CircularBuffer_IsFull(buffer)) {
		buffer->head++;
		if (buffer->head == buffer->capacity) {
			buffer->head = 0;
		}
	}

	// Add the object to the buffer
	buffer->buffer[buffer->tail] = object;

	// Update tail iterator
	buffer->tail++;
	if (buffer->tail == buffer->capacity) {
		buffer->tail = 0;
	}

	return 0;
}

void *GR_CircularBuffer_Pop(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (!buffer) {
		return NULL;
	}

	// Get buffer head's pointer
	void *result = buffer->buffer[buffer->head];

	// Return immediately if buffer is empty
	if (result == NULL) {
		return NULL;
	}

	// Update the buffer
	buffer->buffer[buffer->head] = NULL;

	// Update head iterator
	buffer->head++;
	if (buffer->head == buffer->capacity) {
		buffer->head = 0;
	}

	// Return result
	return result;
}

void *GR_CircularBuffer_Peek(CircularBuffer_Ptr buffer)
{
	// Buffer null error
	if (!buffer) {
		return NULL;
	}
	return buffer->buffer[buffer->head];
}
