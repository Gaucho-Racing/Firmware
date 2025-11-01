#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

// Circular buffer is a FIFO data structure. This struct's fields should not be
// visible to the user.
typedef struct circular_buffer_st CircularBuffer;

/// @brief Creates a CircularBuffer object with the specified capacity.
/// @param capacity The desired capacity of the circular buffer, in
/// number of items. This value must be greater than 0.
/// @return The pointer to the created CircularBuffer object. NULL if capacity
/// specified is 0.
CircularBuffer *GR_CircularBuffer_Create(uint16_t capacity);

/// @brief Frees a CircularBuffer's all associated memories.
///        Note that any pointer stored by this buffer is not
///        freed by this function.
/// @param buffer_pp The pointer to the pointer to the CircularBuffer to free.
void GR_CircularBuffer_Free(CircularBuffer **buffer_pp);

/// @brief Get a CircularBuffer's maximum capacity.
/// @param buffer_ptr The pointer to the CircularBuffer to inspect.
/// @return The buffer's capacity, in number of items. Returns zero if buffer is
/// a null pointer.
uint16_t GR_CircularBuffer_GetCapacity(CircularBuffer *buffer_ptr);

/// @brief Get a CircularBuffer's currently used size.
/// @param buffer_ptr The pointer to the CircularBuffer to inspect.
/// @return The buffer's usage, in number of items stored. Returns 0 if buffer
/// is a null pointer.
uint16_t GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer_ptr);

/// @brief Check if a CircularBuffer is full.
/// @param buffer_ptr The pointer to the CircularBuffer to inspect.
/// @return true if full, false otherwise. Returns true if buffer is a null
/// pointer.
bool GR_CircularBuffer_IsFull(CircularBuffer *buffer_ptr);

/// @brief Check if a CircularBuffer is empty.
/// @param buffer_ptr The pointer to the CircularBuffer to inspect.
/// @return true if empty, false otherwise. Returns true if buffer is a null
/// pointer.
bool GR_CircularBuffer_IsEmpty(CircularBuffer *buffer_ptr);

/// @brief Push a copy of an item into a circular buffer.
/// @param buffer_ptr Pointer to the CircularBuffer to store the item.
/// @param object_ptr A pointer to the item to be pushed into the
/// CircularBuffer.
/// @return 0 if the operation finished successfully, non-0 on error.
uint8_t GR_CircularBuffer_Push(CircularBuffer *buffer_ptr, void *object_ptr);

/// @brief Removes a CircularBuffer's buffer head and retrives it.
/// @param buffer_ptr The CircularBuffer in subject.
/// @return The pointer to the removed item. NULL if the buffer is null or is
/// empty.
void *GR_CircularBuffer_Pop(CircularBuffer *buffer_ptr);

/// @brief Retrives a CircularBuffer's buffer head.
/// @param buffer_ptr The CircularBuffer in subject.
/// @return The pointer to the retrived item. NULL if the buffer is null or is
/// empty.
void *GR_CircularBuffer_Peek(CircularBuffer *buffer_ptr);

#endif
