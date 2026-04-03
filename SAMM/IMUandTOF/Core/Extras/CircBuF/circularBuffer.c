#include "circularBuffer.h"
CircularBuffer *circular_buffer_init(size_t capacity, size_t max_array_size) {
    CircularBuffer *cb = malloc(sizeof(CircularBuffer));
    if (!cb) {
        return NULL;
    }

    cb->buffer = malloc(capacity * sizeof(void *));
    if (!cb->buffer) {
        free(cb);
        return NULL;
    }

    for (size_t i = 0; i < capacity; i++) {
        cb->buffer[i] = malloc(max_array_size);
        if (!cb->buffer[i]) {
            for (size_t j = 0; j < i; j++) {
                free(cb->buffer[j]);
            }
            free(cb->buffer);
            free(cb);
            return NULL;
        }
    }
    cb->capacity = capacity;
    cb->max_arr_size = max_array_size;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    return cb;
}

void circularBufferDestroy(CircularBuffer *cb) {
    if (!cb) return;

    for (size_t i = 0; i < cb->capacity; i++) {
        free(cb->buffer[i]);
    }
    free(cb->buffer);
    free(cb);
}

int circularBufferPush(CircularBuffer *cb, void *array, size_t array_size) {
    if (!cb || cb->size == cb->capacity || array_size > cb->max_arr_size) {
        return -1; // Buffer is full or array is too large
    }

    // Copy the array into the buffer
    memcpy(cb->buffer[cb->head], array, array_size);
    cb->head = (cb->head + 1) % cb->capacity;
    cb->size++;
    return 0;
}

void *circularBufferPop(CircularBuffer *cb) {
    if (!cb || cb->size == 0) {
        return NULL; // Buffer is empty
    }

    void *array = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % cb->capacity;
    cb->size--;
    return array;
}
