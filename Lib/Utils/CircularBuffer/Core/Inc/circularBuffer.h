#ifndef _CIRCULAR_BUFFER__
#define _CIRCULAR_BUFFER__

#include <stdint.h>


typedef struct{
	uint32_t head;
	uint32_t tail; // exclusive
	uint32_t size;
	void** buffer;

	// void (*circularBufferPush)(void* object);
	// TODO: generate hash with all attributes to reinforce data integrety
} circularBuffer;

struct circularBufferFuncs{
	void(*push)(circularBuffer, void*);
	void*(*pop)(circularBuffer);
	void*(*peek)(circularBuffer);
};

void push(circularBuffer buffer, void* object);
void* pop(circularBuffer buffer);
void* peek(circularBuffer buffer);
circularBuffer createCircularBuffer(uint32_t size);
void freeCircularBuffer(circularBuffer buffer);

static struct circularBufferFuncs circularBufferFuncs = {push, pop, peek};

#endif
