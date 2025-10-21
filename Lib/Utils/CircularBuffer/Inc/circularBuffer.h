#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

// Define the new data type CircularBuffer
typedef struct circular_buffer_st CircularBuffer;

// Define the wrapper for CircularBuffer functions
struct circular_buffer_ops_st{
	CircularBuffer*(* const create)(uint32_t capacity);
	void(* const free)(CircularBuffer* buffer);
	void(* const push)(CircularBuffer* buffer, void* object, size_t size);
	void*(* const pop)(CircularBuffer* buffer);
	void*(* const peek)(CircularBuffer* buffer);
};
// Export an instance of this wrapper
extern const struct circular_buffer_ops_st cbOps;

#endif
