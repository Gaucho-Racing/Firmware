#include "circularBuffer.h"
#include "stdint.h"
#include "usart_ll_platform_deps.h"

typedef void (*USART_RxByteCallback)(uint8_t byte);
typedef struct {
	USART_TypeDef *instance;  // i.e. USART1, USART2, etc.
	uint32_t tx_queue_length; // in # of messages
	uint32_t baud_rate;
	USART_RxByteCallback on_rx_byte;
} USARTConfig;

typedef struct usart_handle_st USARTHandle;

USARTHandle *usart_init_peripheral(USARTConfig *config);
void usart_send(USARTHandle *handle, void *object_ptr, uint32_t object_size);
void usart_release(USARTHandle **handle);
