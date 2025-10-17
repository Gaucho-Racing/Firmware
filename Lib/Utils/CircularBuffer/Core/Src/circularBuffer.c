#include "circularBuffer.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void push(circularBuffer buffer, void* object){
	if(buffer.head == buffer.tail){
		free(buffer.buffer[buffer.head]);
		buffer.head++;
		if(buffer.head == buffer.size)
			buffer.head = 0;
	}
	memcpy(buffer.buffer[buffer.tail], object, sizeof(*object));
	buffer.tail++;
	if(buffer.tail == buffer.size)
		buffer.tail = 0;
}

void* pop(circularBuffer buffer){
	void* result = malloc(sizeof(*buffer.buffer[buffer.head]));
	memcpy(result, buffer.buffer[buffer.head], sizeof(*buffer.buffer[buffer.head]));
	free(buffer.buffer[buffer.head]);
	buffer.head++;
	if(buffer.head == buffer.size)
		buffer.head = 0;
	return result;
}

void* peek(circularBuffer buffer){
	void* result = malloc(sizeof(*buffer.buffer[buffer.head]));
	memcpy(result, buffer.buffer[buffer.head], sizeof(*buffer.buffer[buffer.head]));
	return result;
}

circularBuffer createCircularBuffer(uint32_t size){
	circularBuffer buffer;
	buffer.head = 0;
	buffer.tail = 0;
	buffer.size = size;
	buffer.buffer = malloc(size*sizeof(void*));
	return buffer;
}

void freeCircularBuffer(circularBuffer buffer){
	free(buffer.buffer);
}
