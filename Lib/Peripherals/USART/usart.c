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
	USART_RxByteCallback on_rx_byte;
};

USARTHandle *USART1Handle = NULL;
USARTHandle *USART2Handle = NULL;
USARTHandle *USART3Handle = NULL;
USARTHandle *UART4Handle = NULL;
USARTHandle *UART5Handle = NULL;
USARTHandle *LPUART1Handle = NULL;
USARTHandle **usart_get_global_handle_pptr(USART_TypeDef *instance);
IRQn_Type usart_get_irqn(USART_TypeDef *instance);
void usart_irq(USARTHandle *handle);
void USART1_IRQHandler(void) { usart_irq(USART1Handle); }
void USART2_IRQHandler(void) { usart_irq(USART2Handle); }
void USART3_IRQHandler(void) { usart_irq(USART3Handle); }
void UART4_IRQHandler(void) { usart_irq(UART4Handle); }
void UART5_IRQHandler(void) { usart_irq(UART5Handle); }
void LPUART1_IRQHandler(void) { usart_irq(LPUART1Handle); }

USARTHandle *usart_init_handle(USARTConfig *config);
void usart_init_hardware(USARTConfig *config, USARTHandle *handle);
void usart_enable_clocks(USART_TypeDef *instance);

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
	handle_ptr->current_tx_message = NULL;
	handle_ptr->current_tx_index = 0;
	handle_ptr->on_rx_byte = config->on_rx_byte;

	return handle_ptr;
}

void usart_init_hardware(USARTConfig *config, USARTHandle *handle)
{
	// TODO: I'm hardcoding a ton of stuff here,
	// I have no idea what most of this does
	usart_enable_clocks(handle->instance);

	LL_GPIO_Init(config->gpio_port, config->ll_gpio);

	if (handle->instance == LPUART1) {
		LL_LPUART_Init(handle->instance, config->ll_lpuart);
		LL_LPUART_SetTXFIFOThreshold(handle->instance,
					     LL_LPUART_FIFOTHRESHOLD_1_8);
		LL_LPUART_SetRXFIFOThreshold(handle->instance,
					     LL_LPUART_FIFOTHRESHOLD_1_8);
		LL_LPUART_DisableFIFO(handle->instance);
		LL_LPUART_SetWakeUpMethod(handle->instance,
					  LL_LPUART_WAKEUP_IDLELINE);

		LL_LPUART_Enable(handle->instance);

		while ((!(LL_LPUART_IsActiveFlag_TEACK(handle->instance))) ||
		       (!(LL_LPUART_IsActiveFlag_REACK(handle->instance)))) {
		}
	} else {
		LL_USART_Init(handle->instance, config->ll_usart);

		LL_USART_ConfigAsyncMode(handle->instance);

		LL_USART_EnableDirectionTx(handle->instance);
		LL_USART_EnableDirectionRx(handle->instance);
		LL_USART_Enable(handle->instance);

		while (!LL_USART_IsActiveFlag_TEACK(handle->instance) ||
		       !LL_USART_IsActiveFlag_REACK(handle->instance)) {
		}
	}

	// configure interrupt callback
	NVIC_EnableIRQ(usart_get_irqn(handle->instance));

	if (handle->on_rx_byte) {
		// enable receive interrupt (always on)
		LL_USART_EnableIT_RXNE(handle->instance);
	}
}

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

IRQn_Type usart_get_irqn(USART_TypeDef *instance)
{
	if (instance == USART1) {
		return USART1_IRQn;
	} else if (instance == USART2) {
		return USART2_IRQn;
	} else if (instance == USART3) {
		return USART3_IRQn;
	} else if (instance == UART4) {
		return UART4_IRQn;
	} else if (instance == UART5) {
		return UART5_IRQn;
	} else if (instance == LPUART1) {
		return LPUART1_IRQn;
	} else {
		LOGOMATIC("usart_get_irqn: unknown USART instance");
		return NonMaskableInt_IRQn;
	}
}

void usart_enable_clocks(USART_TypeDef *instance)
{
	if (instance == USART1) {
		LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
	} else if (instance == USART2) {
		LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
	} else if (instance == USART3) {
		LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
	} else if (instance == UART4) {
		LL_RCC_SetUARTClockSource(LL_RCC_UART4_CLKSOURCE_PCLK1);
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
	} else if (instance == UART5) {
		LL_RCC_SetUARTClockSource(LL_RCC_UART5_CLKSOURCE_PCLK1);
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5);
	} else if (instance == LPUART1) {
		LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);
		LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1);
	} else {
		LOGOMATIC("usart_enable_clocks: unknown USART instance");
	}
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

	// if ((want to receive) && (data received))
	if (LL_USART_IsEnabledIT_RXNE(handle->instance) &&
	    LL_USART_IsActiveFlag_RXNE(handle->instance)) {
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

	char byte =
	    handle->current_tx_message->data[handle->current_tx_index++];
	LL_USART_TransmitData8(handle->instance, byte);
}

void usart_rx_ready_callback(USARTHandle *handle)
{
	if (handle->on_rx_byte) {
		uint8_t byte = LL_USART_ReceiveData8(handle->instance);
		handle->on_rx_byte(byte);
	}
}

void usart_release(USARTHandle **handle)
{
	usart_release_hardware(handle);
	usart_release_handle(handle);
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

	// unregister callback
	NVIC_DisableIRQ(usart_get_irqn(instance));

	// disable USART
	LL_USART_Disable(instance);

	// wait for hardware to acknowledge that usart is disabled
	while (LL_USART_IsActiveFlag_TEACK(instance) ||
	       LL_USART_IsActiveFlag_REACK(instance)) {
	}
}
