#include "usart.h"
#include "Logomatic.h"
#include "stdlib.h"
#include "string.h"
#include "usart_ll_platform_deps.h"

typedef struct {
	uint8_t *data;
	uint32_t size;
} USARTMessage;

struct usart_handle_st {
	USART_TypeDef *instance;
	CircularBuffer *tx_buffer;
	USARTMessage *current_tx_message;
	uint32_t current_tx_index;
};

USARTHandle *USART1Handle = NULL;
USARTHandle *USART2Handle = NULL;
USARTHandle *USART3Handle = NULL;
USARTHandle *UART4Handle = NULL;
USARTHandle *UART5Handle = NULL;
USARTHandle *LPUART1Handle = NULL;
USARTHandle **usart_get_global_handle_pptr(USART_TypeDef *instance)
{
	if (instance == USART1) {
		return &USART1Handle;
	} else if (instance == USART2) {
		return &USART2Handle;
	} else if (instance == USART3) {
		return &USART3Handle;
	} else if (instance == UART4) {
		return &UART4Handle;
	} else if (instance == UART5) {
		return &UART5Handle;
	} else if (instance == LPUART1) {
		return &LPUART1Handle;
	} else {
		LOGOMATIC(
		    "usart_get_global_handle_pptr: unknown USART instance");
		return NULL;
	}
}
void usart_irq(USARTHandle *handle);
void USART1_IRQHandler(void) { usart_irq(USART1Handle); }
void USART2_IRQHandler(void) { usart_irq(USART2Handle); }
void USART3_IRQHandler(void) { usart_irq(USART3Handle); }
void UART4_IRQHandler(void) { usart_irq(UART4Handle); }
void UART5_IRQHandler(void) { usart_irq(UART5Handle); }
void LPUART1_IRQHandler(void) { usart_irq(LPUART1Handle); }

USARTHandle *usart_init_handle(USARTConfig *config);
void usart_init_hardware(USARTConfig *config, USARTHandle *handle);

void usart_release_handle(USARTHandle **handle);
void usart_release_hardware(USARTHandle **handle);

void usart_tx_ready_callback(USARTHandle *handle);
void usart_rx_ready_callback(USARTHandle *handle);

USARTHandle *usart_init_peripheral(USARTConfig *config)
{
	USARTHandle *handle_ptr = usart_init_handle(config);
	if (!handle_ptr) {
		return NULL;
	}

	usart_init_hardware(config, handle_ptr);

	return handle_ptr;
}

USARTHandle *usart_init_handle(USARTConfig *config)
{
	// map the instance to the global handle pointer
	USARTHandle **handle_pptr =
	    usart_get_global_handle_pptr(config->instance);
	if (!handle_pptr) {
		LOGOMATIC("usart_allocate_handle: invalid USART instance");
		return NULL;
	}

	// check if the global handle is already allocated
	if (*handle_pptr) {
		LOGOMATIC("usart_init_peripheral: this USART instance "
			  "already in use");
		return NULL;
	}

	// create handle
	USARTHandle *handle_ptr = *handle_pptr = malloc(sizeof(USARTHandle));
	if (!handle_ptr) {
		LOGOMATIC("usart_init_peripheral: malloc failed");
		return NULL;
	}

	handle_ptr->instance = config->instance;
	handle_ptr->tx_buffer =
	    GR_CircularBuffer_Create(config->tx_queue_length);

	return handle_ptr;
}

void usart_init_hardware(USARTConfig *config, USARTHandle *handle)
{
	// Enable GPIOB and USART1 clocks
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	// Select PCLK2 as USART1 clock source
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

	// Configure PB6 = TX, PB7 = RX
	LL_GPIO_InitTypeDef gpio = {0};
	gpio.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	gpio.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio.Alternate = LL_GPIO_AF_7;
	gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio.Pull = LL_GPIO_PULL_UP;
	gpio.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOB, &gpio);

	// Configure USART
	LL_USART_InitTypeDef us = {0};
	us.PrescalerValue = LL_USART_PRESCALER_DIV1;
	us.BaudRate = config->baud_rate;
	us.DataWidth = LL_USART_DATAWIDTH_8B;
	us.StopBits = LL_USART_STOPBITS_1;
	us.Parity = LL_USART_PARITY_NONE;
	us.TransferDirection = LL_USART_DIRECTION_TX_RX;
	us.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	us.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(handle->instance, &us);

	LL_USART_ConfigAsyncMode(handle->instance);

	// Enable TX/RX explicitly and enable USART
	LL_USART_EnableDirectionTx(handle->instance);
	LL_USART_EnableDirectionRx(handle->instance);
	LL_USART_Enable(handle->instance);

	// Wait for hardware to acknowledge that usart is enabled
	while (!LL_USART_IsActiveFlag_TEACK(handle->instance) ||
	       !LL_USART_IsActiveFlag_REACK(handle->instance)) {
	}

	// enable receive interrupt
	LL_USART_EnableIT_RXNE(handle->instance);
}

// queue a message to be sent
void usart_send(USARTHandle *handle, void *object_ptr, uint32_t object_size)
{
	if (object_size == 0 || object_ptr == NULL) {
		LOGOMATIC("usart_send called with no data!?");
		return;
	}

	// create message
	USARTMessage msg;
	msg.size = object_size;
	msg.data = malloc(object_size);
	if (!msg.data) {
		LOGOMATIC("usart_send: malloc failed");
		return;
	}
	memcpy(msg.data, object_ptr, object_size);

	// wait for space in the buffer
	while (GR_CircularBuffer_IsFull(handle->tx_buffer)) {
		LOGOMATIC("USART TX buffer full, waiting to send data");
		// Okay to wait here for a while because if this is
		// happening then user code is misbehaving and we don't
		// want to flood the log
		LL_mDelay(100);
	}

	// push message
	GR_CircularBuffer_Push(handle->tx_buffer, &msg, sizeof(msg));

	// enable TXE interrupt since we definitely have something to send now
	// (it's fine if it was already enabled)
	LL_USART_EnableIT_TXE(handle->instance);
}

void usart_irq(USARTHandle *handle)
{
	// if ((want to transmit) && (ready to transmit))
	if (LL_USART_IsEnabledIT_TXE(handle->instance) &&
	    LL_USART_IsActiveFlag_TXE(handle->instance)) {
		usart_tx_ready_callback(handle);
	}

	// if (data received)
	if (LL_USART_IsActiveFlag_RXNE(handle->instance)) {
		usart_rx_ready_callback(handle);
	}
}

void usart_tx_ready_callback(USARTHandle *handle)
{
	// get first message if there is none currently being sent
	if (!handle->current_tx_message) {
		if (GR_CircularBuffer_IsEmpty(handle->tx_buffer)) {
			// nothing more to send, disable interrupt
			LL_USART_DisableIT_TXE(handle->instance);
			return;
		}

		handle->current_tx_message =
		    GR_CircularBuffer_Pop(handle->tx_buffer);
		handle->current_tx_index = 0;
	}

	// if current message is complete, advance to next message
	if (handle->current_tx_index >= handle->current_tx_message->size) {
		// free completed message
		free(handle->current_tx_message->data);
		free(handle->current_tx_message);

		// advance to next message
		handle->current_tx_message =
		    GR_CircularBuffer_Pop(handle->tx_buffer);
		handle->current_tx_index = 0;

		// recurse just in case the next message is also complete or
		// non-existent
		usart_tx_ready_callback(handle);
		return;
	}

	LL_USART_TransmitData8(
	    handle->instance,
	    handle->current_tx_message->data[handle->current_tx_index++]);
}

void usart_rx_ready_callback(USARTHandle *handle)
{
	uint8_t byte = LL_USART_ReceiveData8(handle->instance);
	LOGOMATIC("USART received byte: 0x%02X ('%c')\n", byte,
		  (byte >= 32 && byte <= 126) ? byte : '.');

#ifndef LOGOMATIC_ENABLED
	byte = byte; // suppress unused variable warning
#endif
}

void usart_release(USARTHandle **handle)
{
	usart_release_handle(handle);
	usart_release_hardware(handle);
}

void usart_release_handle(USARTHandle **handle_pptr)
{
	if (!handle_pptr || !*handle_pptr) {
		return;
	}
	USARTHandle *handle_ptr = *handle_pptr;

	// free TX buffer
	{
		// each message in the tx buffer
		USARTMessage *msg;
		while ((msg = GR_CircularBuffer_Pop(handle_ptr->tx_buffer))) {
			free(msg->data);
			free(msg);
		}

		// the buffer itself
		GR_CircularBuffer_Free(&handle_ptr->tx_buffer);
	}

	// free current message if any
	if (handle_ptr->current_tx_message) {
		free(handle_ptr->current_tx_message->data);
		free(handle_ptr->current_tx_message);
	}

	// free the handle
	free(handle_ptr);
	*handle_pptr = NULL;
}

void usart_release_hardware(USARTHandle **handle)
{
	if (!handle || !*handle) {
		return;
	}
	USART_TypeDef *instance = (*handle)->instance;

	// disable interrupts (it's okay if they were already disabled)
	LL_USART_DisableIT_TXE(instance);
	LL_USART_DisableIT_RXNE(instance);

	// disable USART
	LL_USART_Disable(instance);

	// wait for hardware to acknowledge that usart is disabled
	while (LL_USART_IsActiveFlag_TEACK(instance) ||
	       LL_USART_IsActiveFlag_REACK(instance)) {
	}
}