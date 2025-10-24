#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

// Circular buffer is a FIFO data structure.
typedef struct circular_buffer_st CircularBuffer;

// Specifies certain behaviours for a CircularBuffer. Possible values are
// CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE and
// CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE
typedef enum {
	// @brief In this mode, when an item is overwritten by a push command,
	// the item's memory is not automatically freed.
	CIRCULAR_BUFFER_KEEP_ITEM_ON_OVERWRITE,
	// @brief In this mode, when an item is overwritten by a push command,
	// the item's memory is automatically freed.
	CIRCULAR_BUFFER_FREE_ITEM_ON_OVERWRITE
} CircularBufferMode;

/// @brief Creates a CircularBuffer object with the specified capacity.
/// @param capacity The desired capacity of the circular buffer, in
/// number of items.
/// @param mode The operating mode of the buffer, see the enum's definition.
/// @return The pointer to the created CircularBuffer object.
CircularBuffer *GR_CircularBuffer_Create(uint16_t capacity,
					 CircularBufferMode mode);

/// @brief Frees a CircularBuffer's all associated memories.
///        Note that any pointers defined within each items are not
///        freed by this function.
/// @param buffer The pointer to the CircularBuffer to free.
void GR_CircularBuffer_Free(CircularBuffer *buffer);

/// @brief Get a CircularBuffer's maximum capacity.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return The buffer's capacity, in number of items.
uint16_t GR_CircularBuffer_GetCapacity(CircularBuffer *buffer);

/// @brief Get a CircularBuffer's currently used size.
/// @param buffer The pointer to the CircularBuffer to inspect.
/// @return The buffer's usage, in number of items stored.
uint16_t GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer);

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
void GR_CircularBuffer_Push(CircularBuffer *buffer, void *object);

/// @brief Removes a CircularBuffer's buffer head and retrives it.
/// @param buffer The CircularBuffer in subject.
/// @return The pointer to the removed item.
void *GR_CircularBuffer_Pop(CircularBuffer *buffer);

/// @brief Retrives a CircularBuffer's buffer head.
/// @param buffer The CircularBuffer in subject.
/// @return The pointer to the retrived item.
void *GR_CircularBuffer_Peek(CircularBuffer *buffer);

#endif
