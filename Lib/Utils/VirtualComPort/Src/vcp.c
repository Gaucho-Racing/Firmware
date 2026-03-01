#include "vcp.h"

#include <stdbool.h>

#include "Logomatic.h"
#include "main.h"

static bool is_initialized = false;
static VCP_Config vcp_config = {0};

void VCP_Send(uint8_t *data, uint32_t length)
{
	if (!is_initialized) {
		LOGOMATIC("VCP_Send called before successful initialization\n");
		return;
	}
	if (data == NULL) {
		LOGOMATIC("Data pointer is NULL in VCP_Send\n");
		return;
	}
	if (length == 0) {
		LOGOMATIC("Length is 0 in VCP_Send\n");
		return;
	}
	for (uint32_t i = 0; i < length; i++) {
		while (!LL_USART_IsActiveFlag_TXE(vcp_config.usart_instance)) {}
		LL_USART_TransmitData8(vcp_config.usart_instance, data[i]);
	}
}

bool VCP_IsDataAvailable()
{
	if (!is_initialized) {
		LOGOMATIC("VCP_IsDataAvailable called before successful initialization\n");
		return false;
	}
	return LL_USART_IsActiveFlag_RXNE(vcp_config.usart_instance);
}

uint8_t VCP_Receive(void)
{
	if (!is_initialized) {
		LOGOMATIC("VCP_Receive called before successful initialization\n");
		return BAD_OP;
	}
	if (!VCP_IsDataAvailable()) {
		LOGOMATIC("VCP_Receive called when no data is available\n");
		return BAD_OP;
	}
	return LL_USART_ReceiveData8(vcp_config.usart_instance);
}

uint8_t VCP_ReceiveLine(uint8_t *buffer, uint8_t buffer_size)
{
	if (!is_initialized) {
		LOGOMATIC("VCP_ReceiveLine called before successful initialization\n");
		return 0;
	}
	if (buffer == NULL) {
		LOGOMATIC("Buffer pointer is NULL in VCP_ReceiveLine\n");
		return 0;
	}
	if (buffer_size == 0) {
		LOGOMATIC("Buffer size must be greater than 0 in VCP_ReceiveLine\n");
		return 0;
	}
	uint8_t index = 0;
	while (index < buffer_size - 1) {
		while (!LL_USART_IsActiveFlag_RXNE(vcp_config.usart_instance)) {}
		uint8_t received_char = LL_USART_ReceiveData8(vcp_config.usart_instance);
		if ((received_char == '\n') || (received_char == '\0')) {
			break;
		}
		buffer[index++] = received_char;
	}
	buffer[index] = '\0';
	return index;
}

void Setup_VCP(VCP_Config *input_config)
{
	if (input_config == NULL) {
		LOGOMATIC("Input config pointer is NULL in Setup_VCP\n");
		return;
	}
	vcp_config = *input_config;

	if (vcp_config.usart_instance == NULL) {
		LOGOMATIC("USART instance is NULL in Setup_VCP\n");
		return;
	}

	if (vcp_config.baud_rate == 0) {
		LOGOMATIC("Baud rate must be greater than 0 in Setup_VCP\n");
		return;
	}

	if (vcp_config.usart_instance == USART2) {
		switch (vcp_config.clock_source) {
			case VCP_CLOCK_PCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
				break;
			case VCP_CLOCK_SYSCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_SYSCLK);
				break;
			case VCP_CLOCK_HSI:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_HSI);
				break;
			case VCP_CLOCK_LSE:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_LSE);
				break;
			default:
				LOGOMATIC("Unsupported clock source for USART2 in Setup_VCP\n");
				return;
		}
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
	} else if (vcp_config.usart_instance == USART3) {
		switch (vcp_config.clock_source) {
			case VCP_CLOCK_PCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
				break;
			case VCP_CLOCK_SYSCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
				break;
			case VCP_CLOCK_HSI:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_HSI);
				break;
			case VCP_CLOCK_LSE:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_LSE);
				break;
			default:
				LOGOMATIC("Unsupported clock source for USART3 in Setup_VCP\n");
				return;
		}
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

	} else {
		LOGOMATIC("Unsupported USART instance for Setup_VCP\n");
		return;
	}

	LL_AHB2_GRP1_EnableClock(vcp_config.bus_port);

	LL_GPIO_InitTypeDef GPIO_InitStruct = {.Pin = vcp_config.gpio_tx_rx_pin_mask,
					       .Mode = LL_GPIO_MODE_ALTERNATE,
					       .Speed = LL_GPIO_SPEED_FREQ_LOW,
					       .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
					       .Pull = LL_GPIO_PULL_NO,
					       .Alternate = LL_GPIO_AF_7};

	switch (vcp_config.bus_port) {
		case VCP_Port_A:
			LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			break;
		case VCP_Port_B:
			LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
			break;
		case VCP_Port_C:
			LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
			break;
		case VCP_Port_D:
			LL_GPIO_Init(GPIOD, &GPIO_InitStruct);
			break;
		case VCP_Port_E:
			LL_GPIO_Init(GPIOE, &GPIO_InitStruct);
			break;
		case VCP_Port_F:
			LL_GPIO_Init(GPIOF, &GPIO_InitStruct);
			break;
		case VCP_Port_G:
			LL_GPIO_Init(GPIOG, &GPIO_InitStruct);
			break;
		default:
			LOGOMATIC("Unsupported GPIO port for Setup_VCP\n");
			return;
	}

	LL_USART_InitTypeDef USART_InitStruct = {.PrescalerValue = vcp_config.prescaler,
						 .BaudRate = vcp_config.baud_rate,
						 .DataWidth = LL_USART_DATAWIDTH_8B,
						 .StopBits = vcp_config.stop_bits,
						 .Parity = vcp_config.parity,
						 .TransferDirection = LL_USART_DIRECTION_TX_RX,
						 .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
						 .OverSampling = vcp_config.oversampling};
	LL_USART_Init(vcp_config.usart_instance, &USART_InitStruct);
	LL_USART_SetTXFIFOThreshold(vcp_config.usart_instance, vcp_config.tx_fifo_threshold);
	LL_USART_SetRXFIFOThreshold(vcp_config.usart_instance, vcp_config.rx_fifo_threshold);
	LL_USART_DisableFIFO(vcp_config.usart_instance);
	LL_USART_ConfigAsyncMode(vcp_config.usart_instance);

	if (vcp_config.usart_instance == USART2) {
		NVIC_EnableIRQ(USART2_IRQn);
		LL_USART_EnableIT_RXNE(USART2);
		LL_USART_Enable(USART2);
		while ((!(LL_USART_IsActiveFlag_TEACK(USART2))) || (!(LL_USART_IsActiveFlag_REACK(USART2)))) {}
	} else if (vcp_config.usart_instance == USART3) {
		NVIC_EnableIRQ(USART3_IRQn);
		LL_USART_EnableIT_RXNE(USART3);
		LL_USART_Enable(USART3);
		while ((!(LL_USART_IsActiveFlag_TEACK(USART3))) || (!(LL_USART_IsActiveFlag_REACK(USART3)))) {}
	} else {
		LOGOMATIC("Unsupported USART instance for Setup_VCP\n");
		return;
	}

	is_initialized = true;
}
