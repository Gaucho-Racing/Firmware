#include "circularBuffer.h"
#include "stdint.h"

typedef struct {
	uint32_t tx_queue_length; // in # of messages
} USARTConfig;

typedef struct usart_handle_st USARTHandle;

USARTHandle usart_init_peripheral(USARTConfig *config);
void usart_send(USARTHandle *handle, void *object_ptr, uint32_t object_size);
uint32_t usart_receive(USARTHandle *handle, uint8_t *buffer, uint32_t size,
		       uint32_t timeout);
void usart_release(USARTHandle *handle);
