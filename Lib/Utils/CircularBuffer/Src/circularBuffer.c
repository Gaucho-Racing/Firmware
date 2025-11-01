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

CircularBuffer *GR_CircularBuffer_Create(uint16_t capacity)
{
	// Return null pointer if an invalid size(< 1) is specified
	if (capacity < 1) {
		return NULL;
	}
	// For specifications see the header file
	CircularBuffer *buffer = malloc(sizeof(CircularBuffer));
	buffer->head = 0;
	buffer->tail = 0;
	buffer->capacity = capacity;
	buffer->buffer = calloc(capacity, sizeof(void *));
	return buffer;
}

void GR_CircularBuffer_Free(CircularBuffer **buffer_pp)
{
	// Error check
	if (!buffer_pp || !*buffer_pp) {
		return;
	}
	// Free buffer body
	free((*buffer_pp)->buffer);
	// Free buffer instance
	free(*buffer_pp);

	*buffer_pp = NULL;
}

uint16_t GR_CircularBuffer_GetCapacity(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return 0;
	}

	return buffer_ptr->capacity;
}

uint16_t GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return 0;
	}
	// Empty case, since occupied member must have a non-null wrapper
	// pointer
	if (buffer_ptr->buffer[buffer_ptr->head] == NULL) {
		return 0;
	}
	// Account for circular buffer's loopback behaviour
	if (buffer_ptr->tail <= buffer_ptr->head) {
		return buffer_ptr->tail + buffer_ptr->capacity - buffer_ptr->head;
	}
	// Normal case
	return buffer_ptr->tail - buffer_ptr->head;
}

bool GR_CircularBuffer_IsFull(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return false;
	}
	// In the case where head and tail iterators are equal,
	// the buffer is full if any entry in the buffer is occupied.
	return buffer_ptr->head == buffer_ptr->tail && buffer_ptr->buffer[0] != NULL;
}

bool GR_CircularBuffer_IsEmpty(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return false;
	}
	// In the case where head and tail iterators are equal,
	// the buffer is empty if any entry in the buffer is free.
	return buffer_ptr->head == buffer_ptr->tail && buffer_ptr->buffer[0] == NULL;
}

uint8_t GR_CircularBuffer_Push(CircularBuffer *buffer_ptr, void *object)
{
	// Buffer null error
	if (!buffer_ptr) {
		return 1;
	}
	// Remove the buffer head if it's going to be overwritten
	// That is, if the buffer is already full
	if (GR_CircularBuffer_IsFull(buffer_ptr)) {
		buffer_ptr->head++;
		if (buffer_ptr->head == buffer_ptr->capacity) {
			buffer_ptr->head = 0;
		}
	}

	// Add the object to the buffer
	buffer_ptr->buffer[buffer_ptr->tail] = object;

	// Update tail iterator
	buffer_ptr->tail++;
	if (buffer_ptr->tail == buffer_ptr->capacity) {
		buffer_ptr->tail = 0;
	}

	return 0;
}

void *GR_CircularBuffer_Pop(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return NULL;
	}

	// Get buffer head's pointer
	void *result = buffer_ptr->buffer[buffer_ptr->head];

	// Return immediately if buffer is empty
	if (result == NULL) {
		return NULL;
	}

	// Update the buffer
	buffer_ptr->buffer[buffer_ptr->head] = NULL;

	// Update head iterator
	buffer_ptr->head++;
	if (buffer_ptr->head == buffer_ptr->capacity) {
		buffer_ptr->head = 0;
	}

	// Return result
	return result;
}

void *GR_CircularBuffer_Peek(CircularBuffer *buffer_ptr)
{
	// Buffer null error
	if (!buffer_ptr) {
		return NULL;
	}
	return buffer_ptr->buffer[buffer_ptr->head];
}
