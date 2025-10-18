#include "circularBuffer.h"
#include <stdlib.h>
#include <string.h>

// Define the circularBuffer data type
struct circular_buffer_st{
	uint32_t head; // the head position inclusive
	uint32_t tail; // the tail position exclusive
	uint32_t size; // the number of items in buffer
	void** buffer; // the buffer body

	// Idea: generate hash with all attributes to reinforce data integrety
};

void cb_push(CircularBuffer* buffer, void* object){
	// Remove the buffer head if it's going to be overwritten
	// That is, if the buffer is already full
	if(buffer->buffer[buffer->head] != NULL && buffer->head == buffer->tail){
		free(buffer->buffer[buffer->head]);
		buffer->head++;
		if(buffer->head == buffer->size)
			buffer->head = 0;
	}
	// Copy object contents to the buffer
	buffer->buffer[buffer->tail] = malloc(sizeof(*object));
	memcpy(buffer->buffer[buffer->tail], object, sizeof(*object));
	// Update tail iterator
	buffer->tail++;
	if(buffer->tail == buffer->size)
		buffer->tail = 0;
}

void* cb_pop(CircularBuffer* buffer){
	// Return null pointer if buffer is empty
	if(buffer->buffer[buffer->head] == NULL)
		return NULL;
	// Copy buffer head to result
	void* result = malloc(sizeof(*buffer->buffer[buffer->head]));
	memcpy(result, buffer->buffer[buffer->head], sizeof(*buffer->buffer[buffer->head]));
	// Remove buffer head
	free(buffer->buffer[buffer->head]);
	buffer->buffer[buffer->head] = NULL;
	// Update head iterator
	buffer->head++;
	if(buffer->head == buffer->size)
		buffer->head = 0;
	// Return result
	return result;
}

void* cb_peek(CircularBuffer* buffer){
	// Return null pointer if buffer is empty
	if(buffer->buffer[buffer->head] == NULL)
		return NULL;
	// Copy buffer head to result and return it
	void* result = malloc(sizeof(*buffer->buffer[buffer->head]));
	memcpy(result, buffer->buffer[buffer->head], sizeof(*buffer->buffer[buffer->head]));
	return result;
}

CircularBuffer* cb_create(uint32_t size){
	// Return null pointer if an invalid size(< 1) is specified
	if(size < 1)
		return NULL;
	// For specifications see the header file
	CircularBuffer* buffer = malloc(sizeof(CircularBuffer));
	buffer->head = 0;
	buffer->tail = 0;
	buffer->size = size;
	buffer->buffer = malloc(size*sizeof(void*));
	return buffer;
}

void cb_free(CircularBuffer* buffer){
	// Free contents
	for(uint32_t i = 0; i < buffer->size; i++)
		free(buffer->buffer[i]);
	// Free buffer body
	free(buffer->buffer);
	// Free buffer instance
	free(buffer);
}

const struct circular_buffer_ops_st cbOps = {cb_create, cb_free, cb_push, cb_pop, cb_peek};
