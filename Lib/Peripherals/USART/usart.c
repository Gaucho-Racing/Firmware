#include "usart.h"
#include "Logomatic.h"
#include "string.h"
#include "usart_ll_platform_deps.h"

typedef struct {
	uint8_t *data;
	uint32_t size;
	uint32_t sent_bytes;
} USARTMessage;

struct usart_handle_st {
	CircularBuffer *tx_buffer;
};

USARTHandle usart_init_peripheral(USARTConfig *config)
{
	USARTHandle handle;

	handle.tx_buffer = GR_CircularBuffer_Create(config->tx_queue_length);

	// Enable GPIOB and USART1 clocks
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	// Select PCLK2 as USART1 clock source
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

	// TODO: need to think about how to (1) register the
	// callback (2) tell the callback which handle to use
	LL_USART_EnableIT_TXE(USART1);

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
	us.BaudRate = 115200;
	us.DataWidth = LL_USART_DATAWIDTH_8B;
	us.StopBits = LL_USART_STOPBITS_1;
	us.Parity = LL_USART_PARITY_NONE;
	us.TransferDirection = LL_USART_DIRECTION_TX_RX;
	us.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	us.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &us);

	LL_USART_ConfigAsyncMode(USART1);

	// Enable TX/RX explicitly and enable USART
	LL_USART_EnableDirectionTx(USART1);
	LL_USART_EnableDirectionRx(USART1);
	LL_USART_Enable(USART1);

	// Wait for TEACK/REACK
	while (!LL_USART_IsActiveFlag_TEACK(USART1) ||
	       !LL_USART_IsActiveFlag_REACK(USART1)) {
	}

	return handle;
}

// queue a message to be sent
void usart_send(USARTHandle *handle, void *object_ptr, uint32_t object_size)
{
	if (object_size == 0 || object_ptr == NULL) {
		LOGOMATIC_Error("usart_send called with no data!?");
		return;
	}

	// create message
	USARTMessage msg;
	msg.size = object_size;
	msg.sent_bytes = 0;
	msg.data = malloc(object_size);
	if (!msg.data) {
		LOGOMATIC_Error("USART TX malloc failed");
		return;
	}
	memcpy(msg.data, object_ptr, object_size);

	// wait for space in TX buffer
	while (GR_CircularBuffer_IsFull(handle->tx_buffer)) {
		LOGOMATIC_Warning("USART TX buffer full, waiting to send data");
		// Okay to wait here for a while because if this is
		// happening then user code is misbehaving and we don't
		// want to flood the log
		LL_mDelay(100);
	}

	// push message
	GR_CircularBuffer_Push(handle->tx_buffer, &msg, sizeof(msg));
}

uint32_t usart_receive(USARTHandle *handle, uint8_t *buffer, uint32_t size,
		       uint32_t timeout)
{
	// suppress unused variable warning (eventually we'll need the handle)
	handle = handle;

	uint32_t bytes_received = 0;
	uint32_t start_tick = HAL_GetTick();
	while (bytes_received < size) {
		if (LL_USART_IsActiveFlag_RXNE(USART1)) {
			buffer[bytes_received++] =
			    LL_USART_ReceiveData8(USART1);
		}
		if ((HAL_GetTick() - start_tick) > timeout) {
			break;
		}
	}
	return bytes_received;
}

void usart_release(USARTHandle *handle)
{
	// free each message in the tx buffer
	USARTMessage *msg;
	while (msg = GR_CircularBuffer_Pop(handle->tx_buffer)) {
		free(msg->data);
		free(msg);
	}

	// free the tx buffer itself
	GR_CircularBuffer_Free(&handle->tx_buffer);
}

uint8_t usart_pop_next_tx_byte(USARTHandle *handle, uint8_t *byte_ptr)
{
	USARTMessage *msg = GR_CircularBuffer_Peek(handle->tx_buffer);
	if (!msg) {
		// no message to send
		return 0;
	}

	if (msg->sent_bytes >= msg->size) {
		// no more data in this message, pop it
		GR_CircularBuffer_Pop(handle->tx_buffer);
		free(msg->data);
		free(msg);

		// the next message could have data though, so try again
		return usart_pop_next_tx_byte(handle, byte_ptr);
	}

	// pop next byte
	*byte_ptr = msg->data[msg->sent_bytes++];
	return 1;
}

void usart_tx_ready_callback(USARTHandle *handle)
{
	uint8_t byte;
	if (usart_pop_next_tx_byte(handle, &byte)) {
		LL_USART_TransmitData8(USART1, byte);
	}
}
