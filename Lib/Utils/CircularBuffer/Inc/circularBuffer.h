#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Define the new data type CircularBuffer
typedef struct circular_buffer_st CircularBuffer;

/// @brief Creates a CircularBuffer object with the specified capacity.
/// @param capacity The desired capacity of the circular buffer, in
/// number of items.
/// @return The pointer to the created CircularBuffer object.
CircularBuffer *GR_CircularBuffer_Create(uint32_t capacity);

/// @brief Frees a CircularBuffer's all associated memories.
///        Note that any pointers defined within each items are not
///        freed by this function.
/// @param buffer The pointer to the CircularBuffer to free.
void GR_CircularBuffer_Free(CircularBuffer *buffer);

/// @brief Get a CircularBuffer's maximum capacity.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return The buffer's capacity, in number of items.
uint32_t GR_CircularBuffer_GetCapacity(CircularBuffer *buffer);

/// @brief Get a CircularBuffer's currently used size.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return The buffer's usage, in number of items stored.
uint32_t GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer);

/// @brief Check if a CircularBuffer is full.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return true if full, false otherwise.
bool GR_CircularBuffer_IsFull(CircularBuffer *buffer);

/// @brief Check if a CircularBuffer is empty.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return true if empty, false otherwise.
bool GR_CircularBuffer_IsEmpty(CircularBuffer *buffer);

/// @brief Push a copy of an item into a circular buffer.
/// @param buffer Pointer to the CircularBuffer to store the item.
/// @param object The item to be pushed into the CircularBuffer.
/// @param size The size of the item, in bytes.
void GR_CircularBuffer_Push(CircularBuffer *buffer, void *object, size_t size);

/// @brief Removes a CircularBuffer's buffer head and retrives it.
/// @param buffer The CircularBuffer in subject.
/// @return The pointer to the removed item.
void *GR_CircularBuffer_Pop(CircularBuffer *buffer);

/// @brief Retrives a CircularBuffer's buffer head.
/// @param buffer The CircularBuffer in subject.
/// @return The pointer to the retrived item.
void *GR_CircularBuffer_Peek(CircularBuffer *buffer);

#endif
