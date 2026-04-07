# Message Buffer

This library implements a FIFO circular buffer to store messages (byte arrays with dynamic length). This circular buffer is itself one giant byte array upon initialization and therefore does not use dynamic message allocation (no external fragmentation). The message buffer has no internal fragmentation, but one should note that for each message pushed into the buffer, a single byte of overhead is used to store the size. Single-byte-sized messages will therefore be 50% space efficient.

The main purpose of this library is to support the SPI peripheral abstraction library (`SPI_Lib`), although any project which needs a circular buffer to store messages (with no memory fragmentation) should use this library as well.

To use this library with CMake, one must link the library `MsgBuffer_Lib` as an `INTERFACE` to their targets with `target_link_libraries()`.

## How To Use

1. Call GR_MsgBuffer_Create(size) with an input argument of the size of the buffer in bytes. This will return a pointer to a struct allocated on the heap.
2. Call GR_MsgBuffer_Push(msg_buffer, byte_array, byte_array_size) to push the byte array elements into the message buffer. Make sure to check if MsgBuffer function calls are successful based on the return value specified in header file.
3. Call GR_MsgBuffer_IsEmpty(msg_buffer) to check if the buffer is empty.
4. Call GR_MsgBuffer_PeekMsgSize(msg_buffer) to check the size of the message at the front of the queue.
5. Call GR_MsgBuffer_Pop(msg_buffer, byte_array) to remove the top message from the circular buffer and place its elements in your provided byte array. Note that the size of the byte array must match the element at the front of the message buffer.
6. Free any byte arrays you created on your own.
7. Clean up the message buffer using GR_MsgBuffer_Free(msg_buffer).

### Limitations

1. The message buffer capacity is statically defined upon creation (although this can be done during runtime and not compile time).
2. The user must always peek the size of the message at the front of the buffer in order to properly pop it. If the user can `guarantee` that all messages in the buffer are of an expected size (static message sizing or predictable message push order), then the user can forgo such additional step. It is implied that if the peeked message size is different than what the user expects, the user must resize their byte array or ensure it has enough space to prevent out-of-bounds memory access.

#### Example Code

```c

// 1. Create the message buffer
GR_MsgBuffer *buf = GR_MsgBuffer_Create(1024);

// 2. Push a byte array
uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
GR_MsgBuffer_Push(buf, data, sizeof(data));

// 3. Check if empty
if (!GR_MsgBuffer_IsEmpty(buf)) {

    // 4. Peek message size and allocate destination
    uint32_t msg_size = GR_MsgBuffer_PeekMsgSize(buf);
    uint8_t *out_data = malloc(msg_size);

    // 5. Pop message into the new array
    if (GR_MsgBuffer_Pop(buf, out_data)) {
        // Use out_data...
    }

    // 6. Free the popped byte array
    free(out_data);
}

// 7. Free the message buffer
GR_MsgBuffer_Free(buf);

```
