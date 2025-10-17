#include "circularBuffer.h"
#include <stdlib.h>
#include <string.h>

void push(circularBuffer buffer, void* object){
	// Remove the buffer head if it's going to be overwritten
	if(buffer.head == buffer.tail){
		free(buffer.buffer[buffer.head]);
		buffer.buffer[buffer.head] = NULL;
		buffer.head++;
		if(buffer.head == buffer.size)
			buffer.head = 0;
	}
	// Copy object contents to the buffer
	buffer.buffer[buffer.tail] = malloc(sizeof(*object));
	memcpy(buffer.buffer[buffer.tail], object, sizeof(*object));
	// Update tail iterator
	buffer.tail++;
	if(buffer.tail == buffer.size)
		buffer.tail = 0;
}

void* pop(circularBuffer buffer){
	// Copy buffer head to result
	void* result = malloc(sizeof(*buffer.buffer[buffer.head]));
	memcpy(result, buffer.buffer[buffer.head], sizeof(*buffer.buffer[buffer.head]));
	// Remove buffer head
	free(buffer.buffer[buffer.head]);
	buffer.buffer[buffer.head] = NULL;
	// Update head iterator
	buffer.head++;
	if(buffer.head == buffer.size)
		buffer.head = 0;
	// Return result
	return result;
}

void* peek(circularBuffer buffer){
	// Copy buffer head to result and return it
	void* result = malloc(sizeof(*buffer.buffer[buffer.head]));
	memcpy(result, buffer.buffer[buffer.head], sizeof(*buffer.buffer[buffer.head]));
	return result;
}

circularBuffer createCircularBuffer(uint32_t size){
	// For specifications see the header file
	circularBuffer buffer;
	buffer.head = 0;
	buffer.tail = 0;
	buffer.size = size;
	buffer.buffer = malloc(size*sizeof(void*));
	return buffer;
}

void freeCircularBuffer(circularBuffer buffer){
	for(uint32_t i = 0; i < buffer.size; i++)
		if(buffer.buffer[i] != NULL){
			free(buffer.buffer[i]);
			buffer.buffer[i] = NULL;
		}
	free(buffer.buffer);
}
