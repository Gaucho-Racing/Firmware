#include "vcp.h"

#include <stdbool.h>
#include <string.h>

#include "Logomatic.h"
#include "main.h"

#ifndef VCP_TX_BUFFER_SIZE
#define VCP_TX_BUFFER_SIZE 128
#endif

static bool is_initialized = false;
static VCP_Config vcp_config = {0};

static uint8_t vcp_tx_buffer[VCP_TX_BUFFER_SIZE] = {0};
static uint32_t vcp_tx_head = 0;
static uint32_t vcp_tx_tail = 0;
static uint32_t vcp_tx_count = 0;

static uint32_t vcp_enter_critical(void)
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

static void vcp_exit_critical(uint32_t primask)
{
	__set_PRIMASK(primask);
}

static void vcp_tx_from_buffer(void)
{
	if (vcp_tx_count == 0) {
		return;
	}

	while (vcp_tx_count > 0 && LL_USART_IsActiveFlag_TXE(vcp_config.usart_instance)) {
		LL_USART_TransmitData8(vcp_config.usart_instance, vcp_tx_buffer[vcp_tx_tail]);
		vcp_tx_tail = (vcp_tx_tail + 1) % VCP_TX_BUFFER_SIZE;
		vcp_tx_count--;
	}
}

void VCP_Send(const char *data, uint32_t length)
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
		uint32_t primask = vcp_enter_critical();
		if (vcp_tx_count >= VCP_TX_BUFFER_SIZE) {
			vcp_tx_tail = (vcp_tx_tail + 1) % VCP_TX_BUFFER_SIZE;
			vcp_tx_count--;
		}

		vcp_tx_buffer[vcp_tx_head] = data[i];
		vcp_tx_head = (vcp_tx_head + 1) % VCP_TX_BUFFER_SIZE;
		vcp_tx_count++;
		vcp_exit_critical(primask);
	}

	uint32_t primask = vcp_enter_critical();
	vcp_tx_from_buffer();
	if (vcp_tx_count > 0 && !LL_USART_IsEnabledIT_TXE(vcp_config.usart_instance)) {
		LL_USART_EnableIT_TXE(vcp_config.usart_instance);
	}
	vcp_exit_critical(primask);
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

	if (vcp_config.usart_instance == USART1) {
		switch (vcp_config.clock_source) {
			case VCP_CLOCK_PCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);
				break;
			case VCP_CLOCK_SYSCLK:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
				break;
			case VCP_CLOCK_HSI:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_HSI);
				break;
			case VCP_CLOCK_LSE:
				LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_LSE);
				break;
			default:
				LOGOMATIC("Unsupported clock source for USART1 in Setup_VCP\n");
				return;
		}
		LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
	} else if (vcp_config.usart_instance == USART2) {
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
					       .Alternate = vcp_config.alternate_function};

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

	if (vcp_config.usart_instance == USART1) {
		NVIC_EnableIRQ(USART1_IRQn);
	} else if (vcp_config.usart_instance == USART2) {
		NVIC_EnableIRQ(USART2_IRQn);
	} else if (vcp_config.usart_instance == USART3) {
		NVIC_EnableIRQ(USART3_IRQn);
	}

	LL_USART_EnableIT_RXNE(vcp_config.usart_instance);
	LL_USART_Enable(vcp_config.usart_instance);
	while ((!(LL_USART_IsActiveFlag_TEACK(vcp_config.usart_instance))) || (!(LL_USART_IsActiveFlag_REACK(vcp_config.usart_instance)))) {}

	is_initialized = true;
}

/**
 * @brief USART interrupt handler for VCP TX/RX processing
 * This function should be called from whichever USART IRQ handler is configured for VCP
 * (USART2_IRQHandler, USART3_IRQHandler, or LPUART1_IRQHandler)
 * It uses vcp_config to determine which USART instance to service.
 */
void VCP_IRQHandler(void)
{
	if (!is_initialized) {
		return;
	}

	if (LL_USART_IsEnabledIT_TXE(vcp_config.usart_instance) && LL_USART_IsActiveFlag_TXE(vcp_config.usart_instance)) {
		uint32_t primask = vcp_enter_critical();
		vcp_tx_from_buffer();
		if (vcp_tx_count == 0) {
			LL_USART_DisableIT_TXE(vcp_config.usart_instance);
		}
		vcp_exit_critical(primask);
	}

	if (LL_USART_IsEnabledIT_RXNE(vcp_config.usart_instance) && LL_USART_IsActiveFlag_RXNE(vcp_config.usart_instance)) {
		char received_data = (char)LL_USART_ReceiveData8(vcp_config.usart_instance);
		if (vcp_config.rx_callback != NULL) {
			vcp_config.rx_callback(received_data);
		}
	}

	if (LL_USART_IsActiveFlag_ORE(vcp_config.usart_instance)) {
		LL_USART_ClearFlag_ORE(vcp_config.usart_instance);
	}
}

__weak void USART1_IRQHandler(void)
{
	VCP_IRQHandler();
}

__weak void USART2_IRQHandler(void)
{
	VCP_IRQHandler();
}

__weak void USART3_IRQHandler(void)
{
	VCP_IRQHandler();
}
