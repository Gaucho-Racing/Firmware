#ifndef _CIRCULAR_BUFFER__
#define _CIRCULAR_BUFFER__

#include <stdint.h>

// Define the circularBuffer data type
typedef struct{
	uint32_t head; // the head position inclusive
	uint32_t tail; // the tail position exclusive
	uint32_t size; // the number of items in buffer
	void** buffer; // the buffer body

	// Idea: generate hash with all attributes to reinforce data integrety
} circularBuffer;

// Define the holder of circularBuffer functions, a static instance of this
// is initialized at the end of this file
struct circularBufferFuncs{
	void(*push)(circularBuffer, void*);
	void*(*pop)(circularBuffer);
	void*(*peek)(circularBuffer);
};

// Buffer manipulation functions defined
void push(circularBuffer buffer, void* object);
void* pop(circularBuffer buffer);
void* peek(circularBuffer buffer);
// Buffer initialization/deinitialization functions defined
circularBuffer createCircularBuffer(uint32_t size);
void freeCircularBuffer(circularBuffer buffer);

// Static instance of functions' container
static struct circularBufferFuncs circularBufferFuncs = {push, pop, peek};

#endif
