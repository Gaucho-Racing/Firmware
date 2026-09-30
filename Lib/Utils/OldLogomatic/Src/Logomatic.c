#include "Logomatic.h"

#include "Unused.h"
#include "main.h"

#if defined(ITM) && defined(LL_GPIO_MODE_ALTERNATE)
/**
 * @brief Initializes the LPUART1 peripheral for use with Logomatic.
 * @param config The configuration for the LPUART1 peripheral.
 */
void Setup_LPUART1_Logomatic(LogomaticConfig *config)
{
	GPIO_TypeDef *gpio_port = 0;
	switch (config->gpio_port) {
		case LOGOMATIC_GPIOA:
			gpio_port = GPIOA;
			break;
		case LOGOMATIC_GPIOB:
			gpio_port = GPIOB;
			break;
		case LOGOMATIC_GPIOC:
			gpio_port = GPIOC;
			break;
		case LOGOMATIC_GPIOD:
			gpio_port = GPIOD;
			break;
		case LOGOMATIC_GPIOE:
			gpio_port = GPIOE;
			break;
		case LOGOMATIC_GPIOF:
			gpio_port = GPIOF;
			break;
		case LOGOMATIC_GPIOG:
			gpio_port = GPIOG;
			break;
		default:
			return;
	}

	LL_LPUART_InitTypeDef LPUART_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_RCC_SetLPUARTClockSource(config->clock_source);
	LL_APB1_GRP2_EnableClock(config->bus);
	LL_AHB2_GRP1_EnableClock(config->gpio_port);
	GPIO_InitStruct.Pin = config->gpio_pin_rx_tx_mask;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
	LL_GPIO_Init(gpio_port, &GPIO_InitStruct);
	LPUART_InitStruct.PrescalerValue = config->prescaler;
	LPUART_InitStruct.BaudRate = config->baud_rate;
	LPUART_InitStruct.DataWidth = config->data_width;
	LPUART_InitStruct.StopBits = config->stop_bits;
	LPUART_InitStruct.Parity = config->parity;
	LPUART_InitStruct.TransferDirection = config->transfer_direction;
	LPUART_InitStruct.HardwareFlowControl = config->hardware_flow_control;
	LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
	LL_LPUART_SetTXFIFOThreshold(LPUART1, config->tx_fifo_threshold);
	LL_LPUART_SetRXFIFOThreshold(LPUART1, config->rx_fifo_threshold);
	LL_LPUART_DisableFIFO(LPUART1);
	LL_LPUART_Enable(LPUART1);

	switch (config->transfer_direction) {
		case LOGOMATIC_DIRECTION_TX:
			while (!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) {}
			break;
		case LOGOMATIC_DIRECTION_RX:
			while (!(LL_LPUART_IsActiveFlag_REACK(LPUART1))) {}
			break;
		case LOGOMATIC_DIRECTION_TX_RX:
			while ((!(LL_LPUART_IsActiveFlag_TEACK(LPUART1))) || (!(LL_LPUART_IsActiveFlag_REACK(LPUART1)))) {}
			break;
		default:
			while (1) {}
	}
}
#endif

#if defined(LOGOMATIC_ENABLED) && defined(ITM)
/**
 * @brief ITM console output function.
 * @param ch The character to send to the ITM console.
 * @return The character that was sent.
 * @note This function is only enabled when LOGOMATIC_ENABLED is defined and ITM is available. Otherwise, it will be compiled out.
 * @note This function is designed to be used with the ITM console, as a result printing floats is an intensive operation.
 */
__attribute__((weak)) int __io_putchar(int ch)
{
	ITM_SendChar(ch);
	return ch;
}
#endif

/**
 * @brief Initializes the Logomatic logging system.
 * @note This function must be called before using LOGOMATIC to ensure that the ITM console is properly configured.
 * @note This function responds correctly to LOGOMATIC_ENABLED, if it is not defined then this function will be compiled out.
 */
void Setup_Logomatic(LogomaticConfig *config)
{
#if defined(LOGOMATIC_ENABLED) && defined(ITM) && defined(LL_GPIO_MODE_ALTERNATE)
	Setup_LPUART1_Logomatic(config);
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	ITM->TER |= (1UL << 0);
	ITM->TCR |= ITM_TCR_ITMENA_Msk;
#elif defined(LOGOMATIC_ENABLED)
#pragma message(                                                                                                                                                                                       \
    "Logomatic is only supported on platforms with ITM and the full LL driver. Logomatic functions will be compiled out. You can still use LOGOMATIC if you set up LPUART and enable ITM by hand.")
	UNUSED(config);
#else
	UNUSED(config);
#endif
}
