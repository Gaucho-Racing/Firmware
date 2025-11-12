#include "usart.h"
#include "Logomatic.h"
#include "string.h"

#include "usart_ll_platform_deps.h"

USARTHandle usart_init_peripheral(USARTConfig *config)
{
	USARTHandle handle;

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

	handle.baud_rate = config->baud_rate;
	return handle;
}

void usart_send(USARTHandle *handle, uint8_t *data, uint32_t size)
{
	// suppress unused variable warning (eventually we'll need the handle)
	handle = handle;

	if (size == 0) {
		// sending null terminated string
		size = strlen((char *)data);
	}
	for (uint32_t i = 0; i < size; i++) {
		while (!LL_USART_IsActiveFlag_TXE_TXFNF(USART1)) {
			LOGOMATIC("Waiting for TXE flag...\n");
		}
		LL_USART_TransmitData8(USART1, data[i]);
	}

	// wait for final byte to transmit
	while (!LL_USART_IsActiveFlag_TXE_TXFNF(USART1))
		;
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
	// suppress unused variable warning (eventually we'll need the handle)
	handle = handle;

	LL_USART_Disable(USART1);
}
