#ifndef __circularBuffer_h__
#define __circularBuffer_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    void **buffer;     // Array of void pointers
    uint8_t capacity;   // Maximum number of elements in the buffer
    uint8_t head;       // Index of the next element to write
    uint8_t tail;       // Index of the next element to read
    uint8_t size;       // Current number of elements in the buffer
    uint8_t max_arr_size; // Maximum size of the array
} CircularBuffer;

CircularBuffer *circular_buffer_init(size_t capacity, size_t max_array_size);
void circularBufferDestroy(CircularBuffer *cb);
int circularBufferPush(CircularBuffer *cb, void *array, size_t array_size);
void *circularBufferPop(CircularBuffer *cb);
#endif