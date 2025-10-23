#include "circularBuffer.h"
#include <stdint.h>
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
		Item *item = buffer->buffer[i];
		if (item != NULL) {
			// Free item body
			free(item->ptr); // TODO: Recursively free stored items
					 // if there exists a way.
			// Free item wrapper
			free(item);
		}
	}
	// Free buffer body
	free(buffer->buffer);
	// Free buffer instance
	free(buffer);
}

uint32_t GR_CircularBuffer_GetCapacity(CircularBuffer *buffer)
{
	return buffer->capacity;
}

uint32_t GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer)
{
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

bool GR_CircularBuffer_IsFull(CircularBuffer *buffer)
{
	// In the case where head and tail iterators are equal,
	// the buffer is full if any entry in the buffer is occupied.
	return buffer->head == buffer->tail && buffer->buffer[0] != NULL;
}

bool GR_CircularBuffer_IsEmpty(CircularBuffer *buffer)
{
	// In the case where head and tail iterators are equal,
	// the buffer is empty if any entry in the buffer is free.
	return buffer->head == buffer->tail && buffer->buffer[0] == NULL;
}

void GR_CircularBuffer_Push(CircularBuffer *buffer, void *object, size_t size)
{
	// Remove the buffer head if it's going to be overwritten
	// That is, if the buffer is already full
	{
		Item *head = buffer->buffer[buffer->head];
		if (head != NULL && buffer->head == buffer->tail) {
			free(head->ptr); // TODO: Make deeper free; see cb_free.
			free(head);
			buffer->head++;
			if (buffer->head == buffer->capacity) {
				buffer->head = 0;
			}
		}
	}

	// Copy object contents to a wrapper
	Item *item = malloc(sizeof(Item)); // allocate wrapper memory
	item->ptr = malloc(size);	   // allocate object memory
	item->size = size;		   // record item size
	memcpy(item->ptr, object, size);   // copy content

	// Add the wrapper to the buffer
	buffer->buffer[buffer->tail] = item;

	// Update tail iterator
	buffer->tail++;
	if (buffer->tail == buffer->capacity) {
		buffer->tail = 0;
	}
}

void *GR_CircularBuffer_Pop(CircularBuffer *buffer)
{
	// Get buffer head's pointer
	Item *head = buffer->buffer[buffer->head];

	// Return null pointer if buffer is empty
	if (head == NULL) {
		return NULL;
	}

	// Copy buffer head to result
	void *result = malloc(head->size);
	memcpy(result, head->ptr, head->size);

	// Remove and free buffer head
	free(head->ptr); // TODO: Make deeper free; see cb_free.
	free(head);
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
	// Get buffer head's pointer
	Item *head = buffer->buffer[buffer->head];

	// Return null pointer if buffer is empty
	if (head == NULL) {
		return NULL;
	}
	// Copy buffer head to result and return it
	void *result = malloc(head->size);
	memcpy(result, head->ptr, head->size);

	// Return result
	return result;
}
