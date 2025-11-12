#include "stdint.h"

typedef struct {
	uint32_t baud_rate;
} USARTConfig;

typedef struct {
	uint32_t baud_rate;
} USARTHandle;

USARTHandle usart_init_peripheral(USARTConfig *config);
void usart_send(USARTHandle *handle, uint8_t *data, uint32_t size);
uint32_t usart_receive(USARTHandle *handle, uint8_t *buffer, uint32_t size,
		       uint32_t timeout);
void usart_release(USARTHandle *handle);
