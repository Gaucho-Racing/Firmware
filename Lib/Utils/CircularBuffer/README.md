# Circular Buffer

This library defines a new type `Circular Buffer` and declares manipulating/inspecting functions.
`Circular Buffer` is a fixed size queue that that stores references to structs allowing for message
buffering.

## Functions
`GR_CircularBuffer_Create(uint16_t capacity) -> CircularBuffer*` take size as the parameter and creates a circular buffer object and returns its memory address.

`GR_CircularBuffer_Free(CircularBuffer **buffer) -> void`: Frees a CircularBuffer's all associated memories. Note that any pointer stored by this buffer is not freed by this function.

`GR_CircularBuffer_GetCapacity(CircularBuffer *buffer) -> uint16_t`: Get a CircularBuffer's maximum capacity.

`GR_CircularBuffer_GetCurrentSize(CircularBuffer *buffer) uint16_t`: Get a CircularBuffer's currently used size.

`GR_CircularBuffer_IsFull(CircularBuffer *buffer) -> bool`: Check if a CircularBuffer is full.

`GR_CircularBuffer_IsEmpty(CircularBuffer *buffer) -> bool`: Check if a CircularBuffer is empty.

`GR_CircularBuffer_Push(CircularBuffer *buffer, void *object) -> int8_t`: Push a copy of an item into a circular buffer.

`GR_CircularBuffer_Pop(CircularBuffer *buffer) -> void*`: Removes a CircularBuffer's buffer head and retrives it.

`GR_CircularBuffer_Peek(CircularBuffer *buffer) -> void*`: Retrives a CircularBuffer's buffer head.
