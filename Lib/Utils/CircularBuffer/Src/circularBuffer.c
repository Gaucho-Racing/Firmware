#include "circularBuffer.h"
#include <stdlib.h>
#include <string.h>

// Define the Item type, a wrapper of stored items
typedef struct {
	void *ptr;   // pointer to the item
	size_t size; // size of the item
} Item;

// Define the circularBuffer data type
struct circular_buffer_st {
	uint32_t head;	   // the head position inclusive
	uint32_t tail;	   // the tail position exclusive
	uint32_t capacity; // the buffer's capacity in number of items
	Item **buffer;	   // the buffer body
};

CircularBuffer *GR_CircularBuffer_Create(uint32_t capacity)
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
	buffer->buffer = calloc(capacity, sizeof(Item *));
	return buffer;
}

void GR_CircularBuffer_Free(CircularBuffer *buffer)
{
	// Error check
	if (buffer == NULL) {
		return;
	}
	// Free contents
	for (uint32_t i = 0; i < buffer->capacity; i++) {
		if (buffer->buffer[i] != NULL) {
			// Free item body
			free(buffer->buffer[i]
				 ->ptr); // TODO: Recursively free stored items
					 // if a method becomes available
			// Free item wrapper
			free(buffer->buffer[i]);
		}
	}
	// Free buffer body
	free(buffer->buffer);
	// Free buffer instance
	free(buffer);
}

void GR_CircularBuffer_Push(CircularBuffer *buffer, void *object, size_t size)
{
	// Remove the buffer head if it's going to be overwritten
	// That is, if the buffer is already full
	if (buffer->buffer[buffer->head] != NULL &&
	    buffer->head == buffer->tail) {
		free(buffer->buffer[buffer->head]
			 ->ptr); // TODO: Make deeper free; see cb_free.
		free(buffer->buffer[buffer->head]);
		buffer->head++;
		if (buffer->head == buffer->capacity) {
			buffer->head = 0;
		}
	}
	// Copy object contents to the buffer
	buffer->buffer[buffer->tail] =
	    malloc(sizeof(Item)); // allocate wrapper memory
	buffer->buffer[buffer->tail]->ptr =
	    malloc(size);			   // allocate object memory
	buffer->buffer[buffer->tail]->size = size; // record item size
	memcpy(buffer->buffer[buffer->tail]->ptr, object, size);
	// Update tail iterator
	buffer->tail++;
	if (buffer->tail == buffer->capacity) {
		buffer->tail = 0;
	}
}

void *GR_CircularBuffer_Pop(CircularBuffer *buffer)
{
	// Return null pointer if buffer is empty
	if (buffer->buffer[buffer->head] == NULL) {
		return NULL;
	}
	// Copy buffer head to result
	void *result = malloc(buffer->buffer[buffer->head]->size);
	memcpy(result, buffer->buffer[buffer->head]->ptr,
	       buffer->buffer[buffer->head]->size);
	// Remove buffer head
	free(buffer->buffer[buffer->head]
		 ->ptr); // TODO: Make deeper free; see cb_free.
	free(buffer->buffer[buffer->head]);
	buffer->buffer[buffer->head] = NULL;
	// Update head iterator
	buffer->head++;
	if (buffer->head == buffer->capacity) {
		buffer->head = 0;
	}
	// Return result
	return result;
}

void *GR_CircularBuffer_Peek(CircularBuffer *buffer)
{
	// Return null pointer if buffer is empty
	if (buffer->buffer[buffer->head] == NULL) {
		return NULL;
	}
	// Copy buffer head to result and return it
	void *result = malloc(buffer->buffer[buffer->head]->size);
	memcpy(result, buffer->buffer[buffer->head]->ptr,
	       buffer->buffer[buffer->head]->size);
	return result;
}
